/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/stack.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <machine/debug.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <armv/context_switch.h>
#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <arch/machine/tlb.h>

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#अन्तर्भाव <benchmark/benchmark_track.h>
#यदि_अन्तः

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#परिभाषय SECTIONS_प्रत्येकम्_SUPER_SECTION BIT(ARMSuper_Section_द्वयङ्काः - ARMSection_द्वयङ्काः)
#परिभाषय पृष्ठानि_प्रत्येकम्_LARGE_PAGE BIT(ARMLarge_Page_द्वयङ्काः - ARMSmall_Page_द्वयङ्काः)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#परिभाषय चरमः_BYTE_PTE(PTE,दीर्घता_2) ((word_t)&(PTE)[(दीर्घता_2)-1] + (BIT(PTE_आकार_द्वयङ्काः)-1))
#परिभाषय चरमः_BYTE_PDE(PDE,दीर्घता_2) ((word_t)&(PDE)[(दीर्घता_2)-1] + (BIT(PDE_आकार_द्वयङ्काः)-1))

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#परिभाषय MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#परिभाषय MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#परिभाषय ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#परिभाषय ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */

स्थिर शून्य समुच्चयः_Global_PD(शून्य);

संरचना resolve_ret {
    paddr_t frame_आधारः;
    vm_page_आकार_t frame_आकार;
    bool_t valid;
};
प्रकारनिर्देश संरचना resolve_ret resolve_ret_t;

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
स्थिर bool_t विशुद्धम् pteCheckIfMapped(pte_t *pte);
स्थिर bool_t विशुद्धम् pdeCheckIfMapped(pde_t *pde);

स्थिर word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    विकल्प (vm_rights) {
    पक्ष VMNoAccess:
        प्रत्यागम 0;

    पक्ष VMKernelOnly:
        प्रत्यागम 1;

    पक्ष VMReadOnly:
        प्रत्यागम 2;

    पक्ष VMReadWrite:
        प्रत्यागम 3;

    स्वभाव:
        fail("Invalid VM rights");
    }
}

#अन्यथा
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_संहिता
स्थिर word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    विकल्प (vm_rights) {
    पक्ष VMKernelOnly:
        प्रत्यागम 0;
    पक्ष VMReadWrite:
        प्रत्यागम 1;
    पक्ष VMNoAccess:
        /* RO at PL1 only */
        प्रत्यागम 2;
    पक्ष VMReadOnly:
        प्रत्यागम 3;
    स्वभाव:
        fail("Invalid VM rights");
    }
}

स्थिर word_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    विकल्प (vm_rights) {
    पक्ष VMKernelOnly:
    पक्ष VMNoAccess:
        प्रत्यागम 0;
    पक्ष VMReadOnly:
        प्रत्यागम 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    पक्ष VMReadWrite:
        प्रत्यागम 3;
    स्वभाव:
        fail("Invalid VM rights");
    }
}

#यदि_अन्तः

vm_rights_t CONST आवरणम्_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_आवरणम्)
{
    यदि (vm_rights == VMNoAccess) {
        प्रत्यागम VMNoAccess;
    }
    यदि (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्)) {
        प्रत्यागम VMReadOnly;
    }
    यदि (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्)) {
        यदि (!seL4_CapRights_get_capAllowWrite(cap_rights_आवरणम्)) {
            प्रत्यागम VMReadOnly;
        } अन्यथा {
            प्रत्यागम VMReadWrite;
        }
    }
    यदि (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_आवरणम्)) {
        userError("Attempted to make unsupported write only mapping");
    }
    प्रत्यागम VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_संहिता शून्य map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_गुणाः_t गुणाः)
{
    word_t idx = (vaddr & आवरणम्(page_द्वयङ्काः_For_आकार(ARMSection))) >> page_द्वयङ्काः_For_आकार(ARMSmallPage);

    प्रतिज्ञा(vaddr >= PPTR_शिखरम्); /* vaddr lies in the region the global PT covers */
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t tex;
    यदि (vm_गुणाः_get_arm_Page_Cacheable(गुणाः)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } अन्यथा {
        tex = 0;
    }
    armKSGlobalPT[idx] =
        pte_pte_small_new(
            paddr,
            0, /* global */
            SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
            0, /* APX = 0, privileged full access */
            tex,
            APFromVMRights(vm_rights),
            0, /* C (Inner write allocate) */
            1, /* B (Inner write allocate) */
            0  /* executable */
        );
#अन्यथा /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    armHSGlobalPT[idx] =
        pteS1_pteS1_small_new(
            0, /* Executable */
            0, /* Executable at PL1 */
            0, /* Not contiguous */
            paddr,
            0, /* global */
            1, /* AF -- always set */
            0, /* Not shared */
            APFromVMRights(vm_rights),
            0, /* non secure */
            vm_गुणाः_get_arm_Page_Cacheable(गुणाः)
        );
#यदि_अन्तः /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_संहिता शून्य map_kernel_गवाक्षः(शून्य)
{
    paddr_t  phys;
    word_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_आधारः();
    idx = PPTR_आधारः >> page_द्वयङ्काः_For_आकार(ARMSection);

    यावत् (idx < BIT(PD_सूचकांक_द्वयङ्काः) - SECTIONS_प्रत्येकम्_SUPER_SECTION) {
        word_t idx2;

        pde = pde_pde_section_new(
                  phys,
                  1, /* SuperSection */
                  0, /* global */
                  SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                  0, /* APX = 0, privileged full access */
                  5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                  1, /* VMKernelOnly */
                  1, /* Parity enabled */
                  0, /* Domain 0 */
                  0, /* XN not set */
                  0, /* C (Inner write allocate) */
                  1  /* B (Inner write allocate) */
              );
        प्रति (idx2 = idx; idx2 < idx + SECTIONS_प्रत्येकम्_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += BIT(page_द्वयङ्काः_For_आकार(ARMSuperSection));
        idx += SECTIONS_प्रत्येकम्_SUPER_SECTION;
    }

    यावत् (idx < (PPTR_शिखरम् >> page_द्वयङ्काः_For_आकार(ARMSection))) {
        pde = pde_pde_section_new(
                  phys,
                  0, /* Section */
                  0, /* global */
                  SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                  0, /* APX = 0, privileged full access */
                  5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                  1, /* VMKernelOnly */
                  1, /* Parity enabled */
                  0, /* Domain 0 */
                  0, /* XN not set */
                  0, /* C (Inner write allocate) */
                  1  /* B (Inner write allocate) */
              );
        armKSGlobalPD[idx] = pde;
        phys += BIT(page_द्वयङ्काः_For_आकार(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    प्रतिज्ञा(phys == PADDR_शिखरम्);

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*स्थानाङ्कः*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, BIT(se_L4_Page_सारणी_द्वयङ्काः));

    phys += BIT(page_द्वयङ्काः_For_आकार(ARMSection));
    idx++;
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*स्थानाङ्कः*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_Page_सारणी_द्वयङ्काः);

    /* map vector table */
    map_kernel_frame(
        addrFromKPPtr(arm_सदिशम्_सारणी),
        PPTR_सदिशम्_सारणी,
        VMKernelOnly,
        vm_गुणाः_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#अन्यथा /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_संहिता शून्य map_kernel_गवाक्षः(शून्य)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t अप्रयुक्तम् pteS2;

    /* Initialise PGD */
    प्रति (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    प्रति (idx = 0; idx < (USER_शिखरम् - 0xC0000000) >> (PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_आधारः();
    प्रति (; idx < BIT(PT_सूचकांक_द्वयङ्काः) - 1; idx++) {
        pde = pdeS1_pdeS1_section_new(
                  0, /* Executable */
                  0, /* Executable in PL1 */
                  0, /* Not contiguous */
                  phys, /* Address */
                  0, /* global */
                  1, /* AF -- always set to 1 */
                  0, /* Not Shareable */
                  0, /* AP: WR at PL1 only */
                  0, /* Not secure */
                  1  /* outer write-back Cacheable */
              );
        armHSGlobalPD[idx] = pde;
        phys += BIT(PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_Page_सारणी_द्वयङ्काः);
    प्रति (idx = 0; idx < 256; idx++) {
        pteS1_t pte;
        pte = pteS1_pteS1_small_new(
                  0, /* Executable */
                  0, /* Executable in PL1 */
                  0, /* Not contiguous */
                  phys, /* Address */
                  0, /* global */
                  1, /* AF -- always set to 1 */
                  0, /* Not Shareable */
                  0, /* AP: WR at PL1 only */
                  0, /* Not secure */
                  1  /* outer write-back Cacheable */
              );
        armHSGlobalPT[idx] = pte;
        phys += BIT(PAGE_द्वयङ्काः);
    }
    /* map vector table */
    map_kernel_frame(
        addrFromKPPtr(arm_सदिशम्_सारणी),
        PPTR_सदिशम्_सारणी,
        VMKernelOnly,
        vm_गुणाः_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#यदि_अन्तः /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

स्थिर BOOT_संहिता शून्य map_it_frame_cap(cap_t pd_cap, cap_t frame_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *targetSlot;
    pde_t *pd    = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    शून्य  *frame = (शून्य *)generic_frame_cap_get_capFBasePtr(frame_cap);
    vptr_t vptr  = generic_frame_cap_get_cap_FMapped_स्थानाङ्कः(frame_cap);

    प्रतिज्ञा(generic_frame_cap_get_capFMappedASID(frame_cap) != 0);

    pd += (vptr >> page_द्वयङ्काः_For_आकार(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_स्थानाङ्कः(pd));
    targetSlot = pt + ((vptr & आवरणम्(page_द्वयङ्काः_For_आकार(ARMSection)))
                       >> page_द्वयङ्काः_For_आकार(ARMSmallPage));
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pte_pte_small_new(
                      addrFromPPtr(frame),
                      1, /* not global */
                      SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                      0, /* APX = 0, privileged full access */
                      5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                      APFromVMRights(VMReadWrite),
                      0, /* C (Inner write allocate) */
                      1, /* B (Inner write allocate) */
                      !executable
                  );
#अन्यथा
    *targetSlot = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(frame),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#यदि_अन्तः
}

/* Create a frame cap for the initial thread. */

स्थिर BOOT_संहिता cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    यदि (use_large)
        प्रत्यागम
            cap_frame_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_नीचः(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_उच्चः(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    अन्यथा
        प्रत्यागम
            cap_small_frame_cap_new(
                ASID_नीचः(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#यदि_परिभाषितम् CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#यदि_अन्तः
                ASID_उच्चः(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

स्थिर BOOT_संहिता शून्य map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(pt_cap);
    pde_t *targetSlot = pd + (vptr >> page_द्वयङ्काः_For_आकार(ARMSection));

    प्रतिज्ञा(cap_page_सारणी_cap_get_cap_PTIs_Mapped(pt_cap));

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*स्थानाङ्कः*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#अन्यथा
    *targetSlot = pde_pde_coarse_new(addrFromPPtr(pt));
#यदि_अन्तः
}

/* Create a page table for the initial thread */

स्थिर BOOT_संहिता cap_t create_it_page_सारणी_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_सारणी_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    यदि (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    प्रत्यागम cap;
}

BOOT_संहिता word_t arch_get_n_paging(v_region_t it_v_reg)
{
    प्रत्यागम get_n_paging(it_v_reg, PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_संहिता cap_t create_it_स्थानाङ्कः_space(cap_t मूलम्_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_Page_Dir_द्वयङ्काः) - 1,
                        addrFromPPtr((शून्य *)rootserver.vspace));
    cap_t pd_cap =
        cap_page_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    प्रति (pt_vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);
         pt_vptr < it_v_reg.अन्तः_2;
         pt_vptr += BIT(PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः)) {
        यदि (!provide_cap(मूलम्_cnode_cap,
                         create_it_page_सारणी_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            प्रत्यागम cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    प्रत्यागम pd_cap;
}

BOOT_संहिता cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    प्रत्यागम create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_संहिता cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    प्रत्यागम cap;
}

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_संहिता शून्य activate_kernel_vspace(शून्य)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    समुच्चयः_Global_PD();
    invalidateLocalTLB();
    तालकम्_TLBEntry(PPTR_आधारः);
    तालकम्_TLBEntry(PPTR_सदिशम्_सारणी);
}

#अन्यथा

BOOT_संहिता शून्य activate_kernel_vspace(शून्य)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    समुच्चयः_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    समुच्चयः_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#यदि 0 /* Can't lock entries on A15 */
    तालकम्_TLBEntry(PPTR_आधारः);
    तालकम्_TLBEntry(PPTR_सदिशम्_सारणी);
#यदि_अन्तः
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= BIT(8);       /* Inner write-back, write-allocate */
    r |= BIT(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= BIT(31);
    समुच्चयः_VTCR(r);
}

#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_संहिता शून्य write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_सम्बन्धि_cap(it_ap_cap));
    ap->array[ASID_नीचः(IT_ASID)] = PDE_PTR(pptr_सम्बन्धि_cap(it_pd_cap));
    armKSASIDTable[ASID_उच्चः(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findPDForASID_ret_t findPDForASID(asid_t asid)
{
    findPDForASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_उच्चः(asid)];
    यदि (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();

        ret.pd = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    pd = poolPtr->array[ASID_नीचः(asid)];
    यदि (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();

        ret.pd = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ret.pd = pd;
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

word_t *विशुद्धम् lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_बफर_Ptr;
    cap_t बफर_Cap;
    vm_rights_t vm_rights;

    w_बफर_Ptr = thread->tcbIPCBuffer;
    बफर_Cap = TCB_PTR_CTE_PTR(thread, tcb_बफर)->cap;

    यदि (unlikely(cap_get_capType(बफर_Cap) != cap_small_frame_cap &&
                 cap_get_capType(बफर_Cap) != cap_frame_cap)) {
        प्रत्यागम रिक्त;
    }
    यदि (unlikely(generic_frame_cap_get_capFIsDevice(बफर_Cap))) {
        प्रत्यागम रिक्त;
    }

    vm_rights = generic_frame_cap_get_capFVMRights(बफर_Cap);
    यदि (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t आधारः_Ptr;
        अचिह्नित पूर्णाङ्क page_द्वयङ्काः;

        आधारः_Ptr = generic_frame_cap_get_capFBasePtr(बफर_Cap);
        page_द्वयङ्काः = page_द्वयङ्काः_For_आकार(generic_frame_cap_get_capFSize(बफर_Cap));
        प्रत्यागम (word_t *)(आधारः_Ptr + (w_बफर_Ptr & आवरणम्(page_द्वयङ्काः)));
    } अन्यथा {
        प्रत्यागम रिक्त;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    यदि (unlikely(cap_get_capType(cap) != cap_small_frame_cap &&
                 cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (unlikely(generic_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(vptr & आवरणम्(se_L4_IPCBuffer_आकार_द्वयङ्काः))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (पूर्णाङ्क)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    अचिह्नित पूर्णाङ्क pd_सूचकांक;

    pd_सूचकांक = vptr >> (PAGE_द्वयङ्काः + PT_सूचकांक_द्वयङ्काः);
    प्रत्यागम pd + pd_सूचकांक;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pdSlot;

    pdSlot = lookupPDSlot(pd, vptr);

    यदि (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);

        ret.ptSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    } अन्यथा {
        pte_t *pt, *ptSlot;
        अचिह्नित पूर्णाङ्क pt_सूचकांक;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_स्थानाङ्कः(pdSlot));
        pt_सूचकांक = (vptr >> PAGE_द्वयङ्काः) & आवरणम्(PT_सूचकांक_द्वयङ्काः);
        ptSlot = pt + pt_सूचकांक;

        ret.ptSlot = ptSlot;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    }
}

स्थिर pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    अचिह्नित पूर्णाङ्क pt_सूचकांक;

    pt_सूचकांक = (vptr >> PAGE_द्वयङ्काः) & आवरणम्(PT_सूचकांक_द्वयङ्काः);
    प्रत्यागम pt + pt_सूचकांक;
}

स्थिर ध्रुव resolve_ret_t default_resolve_ret_t;

स्थिर resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    विकल्प (pde_ptr_get_pdeType(pde)) {
    पक्ष pde_pde_section:
        ret.frame_आधारः = pde_pde_section_ptr_get_स्थानाङ्कः(pde);
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (pde_pde_section_ptr_get_आकार(pde)) {
            ret.frame_आकार = ARMSuperSection;
        } अन्यथा {
            ret.frame_आकार = ARMSection;
        }
#अन्यथा
        यदि (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.frame_आधारः &= ~आवरणम्(page_द्वयङ्काः_For_आकार(ARMSuperSection));
            ret.frame_आकार = ARMSuperSection;
        } अन्यथा {
            ret.frame_आकार = ARMSection;
        }
#यदि_अन्तः
        प्रत्यागम ret;

    पक्ष pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_स्थानाङ्कः(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small:
            ret.frame_आधारः = pte_pte_small_ptr_get_स्थानाङ्कः(pte);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            यदि (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frame_आधारः &= ~आवरणम्(page_द्वयङ्काः_For_आकार(ARMLargePage));
                ret.frame_आकार = ARMLargePage;
            } अन्यथा {
                ret.frame_आकार = ARMSmallPage;
            }
#अन्यथा
            ret.frame_आकार = ARMSmallPage;
#यदि_अन्तः
            प्रत्यागम ret;

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large:
            ret.frame_आधारः = pte_pte_large_ptr_get_स्थानाङ्कः(pte);
            ret.frame_आकार = ARMLargePage;
            प्रत्यागम ret;
#यदि_अन्तः
        स्वभाव:
            भङ्ग;
        }
        भङ्ग;
    }
    }

    ret.valid = false;
    प्रत्यागम ret;
}

स्थिर pte_t CONST makeUserPTE(vm_page_आकार_t page_आकार, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap;

    ap = APFromVMRights(vm_rights);

    विकल्प (page_आकार) {
    पक्ष ARMSmallPage: {
        यदि (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } अन्यथा {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        भङ्ग;
    }

    पक्ष ARMLargePage: {
        यदि (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } अन्यथा {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    0, /* TEX = 0b000, strongly-ordered */
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 0,
                                    1 /* reserved */);
        }
        भङ्ग;
    }

    स्वभाव:
        fail("Invalid PTE frame type");
    }

#अन्यथा

    word_t hap;

    hap = HAPFromVMRights(vm_rights);

    विकल्प (page_आकार) {
    पक्ष ARMSmallPage: {
        यदि (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } अन्यथा {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        भङ्ग;
    }

    पक्ष ARMLargePage: {
        यदि (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } अन्यथा {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        भङ्ग;
    }
    स्वभाव:
        fail("Invalid PTE frame type");
    }
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    प्रत्यागम pte;
}

स्थिर pde_t CONST makeUserPDE(vm_page_आकार_t page_आकार, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, word_t domain,
                               vm_rights_t vm_rights)
{
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap, size2;

    ap = APFromVMRights(vm_rights);
#अन्यथा
    word_t hap, size2;

    (शून्य)domain;
    hap = HAPFromVMRights(vm_rights);
#यदि_अन्तः

    विकल्प (page_आकार) {
    पक्ष ARMSection:
        size2 = 0;
        भङ्ग;

    पक्ष ARMSuperSection:
        size2 = 1;
        भङ्ग;

    स्वभाव:
        fail("Invalid PDE frame type");
    }

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    यदि (cacheable) {
        प्रत्यागम pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } अन्यथा {
        प्रत्यागम pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#अन्यथा
    यदि (cacheable) {
        प्रत्यागम pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } अन्यथा {
        प्रत्यागम pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_मूलम्(cap_t cap)
{
    प्रत्यागम cap_get_capType(cap) == cap_page_directory_cap &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

bool_t CONST isIOSpaceFrameCap(cap_t cap)
{
#यदि_परिभाषितम् CONFIG_TK1_SMMU
    प्रत्यागम cap_get_capType(cap) == cap_small_frame_cap && cap_small_frame_cap_get_capFIsIOSpace(cap);
#अन्यथा
    प्रत्यागम false;
#यदि_अन्तः
}

/** Switch to the empty PD on the reserved HW ASID */
स्थिर शून्य समुच्चयः_Global_PD(शून्य)
{
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#अन्यथा
    /* Before changing the PD ensure all memory accesses have completed */
    dsb();
    /* First switch to global PD on old ASID. Stale TLB entries may exist
       under the old ASID, but no new stale mappings can be added any more. */
    writeTTBR0Ptr(addrFromKPPtr(armKSGlobalPD));
    /* Ensure the PD switch completes before we do anything else */
    isb();
    /* Switch to reserved HW ASID. Only empty/global kernel mappings are
       now available from the TLB. */
    समुच्चयः_Hardware_ASID(hwASIDReserved);
#यदि_अन्तः
}

शून्य समुच्चयः_VMRoot(tcb_t *tcb)
{
    cap_t thread_मूलम्;
    asid_t asid;
    pde_t *pd;
    findPDForASID_ret_t find_ret;

    thread_मूलम् = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    यदि (cap_get_capType(thread_मूलम्) != cap_page_directory_cap ||
        !cap_page_directory_cap_get_capPDIsMapped(thread_मूलम्)) {
        समुच्चयः_Global_PD();
        प्रत्यागम;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_मूलम्));
    asid = cap_page_directory_cap_get_capPDMappedASID(thread_मूलम्);
    find_ret = findPDForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE || find_ret.pd != pd)) {
        समुच्चयः_Global_PD();
        प्रत्यागम;
    }

    armv_contextSwitch(pd, asid);
}

स्थिर bool_t समुच्चयः_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_मूलम्;

    thread_मूलम् = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbVTable)->cap;

    यदि (cap_get_capType(thread_मूलम्) == cap_page_directory_cap &&
        cap_page_directory_cap_get_capPDIsMapped(thread_मूलम्) &&
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_मूलम्)) == pd) {
        प्रत्यागम false;
    }

    armv_contextSwitch(pd, asid);

    प्रत्यागम true;
}

pde_t *page_सारणी_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findPDForASID_ret_t find_ret;
    pde_t pde;
    अचिह्नित पूर्णाङ्क pd_सूचकांक;

    find_ret = findPDForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
        प्रत्यागम रिक्त;
    }

    pd_सूचकांक = vaddr >> (PAGE_द्वयङ्काः + PT_सूचकांक_द्वयङ्काः);
    pde = find_ret.pd[pd_सूचकांक];

    यदि (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_स्थानाङ्कः(pde)) == pt)) {
        प्रत्यागम find_ret.pd;
    } अन्यथा {
        प्रत्यागम रिक्त;
    }
}

स्थिर शून्य invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_उच्चः(asid)];
    प्रतिज्ञा(asidPool);

    pd = asidPool->array[ASID_नीचः(asid)];
    प्रतिज्ञा(pd);

    pd[PD_ASID_SLOT] = pde_pde_invalid_new(0, false);
}

स्थिर pde_t विशुद्धम् आरोपय_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_उच्चः(asid)];
    प्रतिज्ञा(asidPool);

    pd = asidPool->array[ASID_नीचः(asid)];
    प्रतिज्ञा(pd);

    प्रत्यागम pd[PD_ASID_SLOT];
}

स्थिर शून्य storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_उच्चः(asid)];
    प्रतिज्ञा(asidPool);

    pd = asidPool->array[ASID_नीचः(asid)];
    प्रतिज्ञा(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_SLOT] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t findFreeHWASID(शून्य)
{
    word_t hw_asid_विस्थापनम्;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    प्रति (hw_asid_विस्थापनम् = 0;
         hw_asid_विस्थापनम् <= (word_t)((hw_asid_t) - 1);
         hw_asid_विस्थापनम् ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_विस्थापनम्);
        यदि (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            प्रत्यागम hw_asid;
        }
    }

    /* If we've scanned the table without finding a free ASID */
    hw_asid = armKSNextASID;
    invalidateASID(armKSHWASIDTable[hw_asid]);

    /* Flush TLB */
    invalidateTranslationASID(hw_asid);
    armKSHWASIDTable[hw_asid] = asidInvalid;

    /* Increment the NextASID index, skipping the reserved ASID 0 on wrap */
    armKSNextASID++;
    यदि (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    प्रत्यागम hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = आरोपय_HWASID(asid);
    यदि (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        प्रत्यागम pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } अन्यथा {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        प्रत्यागम new_hw_asid;
    }
}

स्थिर शून्य invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = आरोपय_HWASID(asid);
    यदि (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

शून्य unmap_Page_सारणी(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pdSlot;
    अचिह्नित पूर्णाङ्क pd_सूचकांक;

    pd = page_सारणी_Mapped(asid, vaddr, pt);

    यदि (likely(pd != रिक्त)) {
        pd_सूचकांक = vaddr >> (PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);
        pdSlot = pd + pd_सूचकांक;

        *pdSlot = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));
        flush_सारणी(pd, asid, vaddr, pt);
    }
}

शून्य copyGlobalMappings(pde_t *newPD)
{
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t i;
    pde_t *global_pd = armKSGlobalPD;

    प्रति (i = PPTR_आधारः >> ARMSection_द्वयङ्काः; i < BIT(PD_सूचकांक_द्वयङ्काः); i++) {
        यदि (i != PD_ASID_SLOT) {
            newPD[i] = global_pd[i];
        }
    }
#यदि_अन्तः
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    विकल्प (vm_faultType) {
    पक्ष ARMDataAbort: {
        word_t addr, fault;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (स्थानाङ्कः_Translate_S1(addr) & ~आवरणम्(PAGE_द्वयङ्काः)) | (addr & आवरणम्(PAGE_द्वयङ्काः));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#अन्यथा
        addr = getFAR();
        fault = getDFSR();
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        यदि (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            प्रत्यागम EXCEPTION_FAULT;
        }
#यदि_अन्तः
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        प्रत्यागम EXCEPTION_FAULT;
    }

    पक्ष ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (स्थानाङ्कः_Translate_S1(pc) & ~आवरणम्(PAGE_द्वयङ्काः)) | (pc & आवरणम्(PAGE_द्वयङ्काः));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#अन्यथा
        fault = getIFSR();
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
        यदि (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            यदि (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_चरणः
                && !single_चरणः_Fault_गणक_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                प्रत्यागम EXCEPTION_NONE;
            }
            प्रत्यागम EXCEPTION_FAULT;
        }
#यदि_अन्तः
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        प्रत्यागम EXCEPTION_FAULT;
    }

    स्वभाव:
        fail("Invalid VM fault type");
    }
}

शून्य deleteASIDPool(asid_t asid_आधारः, asid_pool_t *pool)
{
    अचिह्नित पूर्णाङ्क विस्थापनम्;

    /* Haskell error: "ASID pool's base must be aligned" */
    प्रतिज्ञा((asid_आधारः & आवरणम्(asid_नीचः_द्वयङ्काः)) == 0);

    यदि (armKSASIDTable[ASID_उच्चः(asid_आधारः)] == pool) {
        प्रति (विस्थापनम् = 0; विस्थापनम् < BIT(asid_नीचः_द्वयङ्काः); विस्थापनम्++) {
            यदि (pool->array[विस्थापनम्]) {
                flushSpace(asid_आधारः + विस्थापनम्);
                invalidateASIDEntry(asid_आधारः + विस्थापनम्);
            }
        }
        armKSASIDTable[ASID_उच्चः(asid_आधारः)] = रिक्त;
        समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
    }
}

शून्य deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_उच्चः(asid)];

    यदि (poolPtr != रिक्त && poolPtr->array[ASID_नीचः(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_नीचः(asid)] = रिक्त;
        समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
    }
}

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
स्थिर pte_t pte_pte_invalid_new(शून्य)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    प्रत्यागम (pte_t) {
        {
            0
        }
    };
}
#यदि_अन्तः

शून्य unmapPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, शून्य *pptr)
{
    findPDForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findPDForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
        प्रत्यागम;
    }

    विकल्प (page_आकार) {
    पक्ष ARMSmallPage: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
            प्रत्यागम;
        }

        यदि (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            प्रत्यागम;
        }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 0)) {
            प्रत्यागम;
        }
#यदि_अन्तः
        यदि (unlikely(pte_pte_small_ptr_get_स्थानाङ्कः(lu_ret.ptSlot) != addr)) {
            प्रत्यागम;
        }

        *(lu_ret.ptSlot) = pte_pte_invalid_new();
        cleanByVA_PoU((word_t)lu_ret.ptSlot, addrFromPPtr(lu_ret.ptSlot));

        भङ्ग;
    }

    पक्ष ARMLargePage: {
        lookupPTSlot_ret_t lu_ret;
        word_t i;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
            प्रत्यागम;
        }
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_large)) {
            प्रत्यागम;
        }
        यदि (unlikely(pte_pte_large_ptr_get_स्थानाङ्कः(lu_ret.ptSlot) != addr)) {
            प्रत्यागम;
        }
#अन्यथा
        यदि (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            प्रत्यागम;
        }
        यदि (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 1)) {
            प्रत्यागम;
        }
        यदि (unlikely(pte_pte_small_ptr_get_स्थानाङ्कः(lu_ret.ptSlot) != addr)) {
            प्रत्यागम;
        }
#यदि_अन्तः

        प्रति (i = 0; i < पृष्ठानि_प्रत्येकम्_LARGE_PAGE; i++) {
            lu_ret.ptSlot[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((word_t)&lu_ret.ptSlot[0],
                            चरमः_BYTE_PTE(lu_ret.ptSlot, पृष्ठानि_प्रत्येकम्_LARGE_PAGE),
                            addrFromPPtr(&lu_ret.ptSlot[0]));

        भङ्ग;
    }

    पक्ष ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(find_ret.pd, vptr);

        यदि (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            प्रत्यागम;
        }
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (unlikely(pde_pde_section_ptr_get_आकार(pd) != 0)) {
#अन्यथा
        यदि (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#यदि_अन्तः
            प्रत्यागम;
        }
        यदि (unlikely(pde_pde_section_ptr_get_स्थानाङ्कः(pd) != addr)) {
            प्रत्यागम;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pd, addrFromPPtr(pd));

        भङ्ग;
    }

    पक्ष ARMSuperSection: {
        pde_t *pd;
        word_t i;

        pd = lookupPDSlot(find_ret.pd, vptr);

        यदि (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            प्रत्यागम;
        }
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (unlikely(pde_pde_section_ptr_get_आकार(pd) != 1)) {
#अन्यथा
        यदि (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#यदि_अन्तः
            प्रत्यागम;
        }
        यदि (unlikely(pde_pde_section_ptr_get_स्थानाङ्कः(pd) != addr)) {
            प्रत्यागम;
        }

        प्रति (i = 0; i < SECTIONS_प्रत्येकम्_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((word_t)&pd[0], चरमः_BYTE_PDE(pd, SECTIONS_प्रत्येकम्_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        भङ्ग;
    }

    स्वभाव:
        fail("Invalid ARM page type");
        भङ्ग;
    }

    /* Flush the page now that the mapping has been updated */
    flushPage(page_आकार, find_ret.pd, asid, vptr);
}

शून्य flushPage(vm_page_आकार_t page_आकार, pde_t *pd, asid_t asid, word_t vptr)
{
    pde_t stored_hw_asid;
    word_t आधारः_addr;
    bool_t मूलम्_switched;

    प्रतिज्ञा((vptr & आवरणम्(page_द्वयङ्काः_For_आकार(page_आकार))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    मूलम्_switched = समुच्चयः_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = आरोपय_HWASID(asid);

    यदि (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        आधारः_addr = vptr & ~आवरणम्(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(आधारः_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        यदि (मूलम्_switched) {
            समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
        }
    }
}

शून्य flush_सारणी(pde_t *pd, asid_t asid, word_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t मूलम्_switched;

    प्रतिज्ञा((vptr & आवरणम्(PT_सूचकांक_द्वयङ्काः + ARMSmall_Page_द्वयङ्काः)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    मूलम्_switched = समुच्चयः_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = आरोपय_HWASID(asid);

    यदि (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        यदि (मूलम्_switched) {
            समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
        }
    }
}

शून्य flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = आरोपय_HWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    यदि (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        प्रत्यागम;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

शून्य invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = आरोपय_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    यदि (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        प्रत्यागम;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

स्थिर अन्तःस्थ bool_t CONST checkVPAlignment(vm_page_आकार_t sz, word_t w)
{
    प्रत्यागम (w & आवरणम्(page_द्वयङ्काः_For_आकार(sz))) == 0;
}

संरचना create_mappings_pte_return {
    exception_t स्थितिः;
    pte_t pte;
    pte_range_t pte_entries;
};
प्रकारनिर्देश संरचना create_mappings_pte_return create_mappings_pte_return_t;

संरचना create_mappings_pde_return {
    exception_t स्थितिः;
    pde_t pde;
    pde_range_t pde_entries;
};
प्रकारनिर्देश संरचना create_mappings_pde_return create_mappings_pde_return_t;

स्थिर create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t आधारः, word_t vaddr, vm_page_आकार_t frame_आकार,
 vm_rights_t vmRights, vm_गुणाः_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    word_t i;

    विकल्प (frame_आकार) {

    पक्ष ARMSmallPage:

        ret.pte_entries.आधारः = रिक्त; /* to avoid uninitialised warning */
        ret.pte_entries.दीर्घता = 1;

        ret.pte = makeUserPTE(ARMSmallPage, आधारः,
                              vm_गुणाः_get_arm_Page_Cacheable(attr),
                              vm_गुणाः_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            प्रत्यागम ret;
        }

        ret.pte_entries.आधारः = lu_ret.ptSlot;
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (unlikely(pte_ptr_get_pteType(ret.pte_entries.आधारः) ==
                     pte_pte_large)) {
#अन्यथा
        यदि (unlikely(pte_ptr_get_pteType(ret.pte_entries.आधारः) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.आधारः))) {
#यदि_अन्तः
            current_syscall_error.type =
                seL4_DeleteFirst;

            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
            प्रत्यागम ret;
        }

        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    पक्ष ARMLargePage:

        ret.pte_entries.आधारः = रिक्त; /* to avoid uninitialised warning */
        ret.pte_entries.दीर्घता = पृष्ठानि_प्रत्येकम्_LARGE_PAGE;

        ret.pte = makeUserPTE(ARMLargePage, आधारः,
                              vm_गुणाः_get_arm_Page_Cacheable(attr),
                              vm_गुणाः_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            प्रत्यागम ret;
        }

        ret.pte_entries.आधारः = lu_ret.ptSlot;

        प्रति (i = 0; i < पृष्ठानि_प्रत्येकम्_LARGE_PAGE; i++) {
            यदि (unlikely(pte_get_pteType(ret.pte_entries.आधारः[i]) ==
                         pte_pte_small)
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.आधारः[i])
#यदि_अन्तः
               ) {
                current_syscall_error.type =
                    seL4_DeleteFirst;

                ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
                प्रत्यागम ret;
            }
        }

        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    स्वभाव:
        fail("Invalid or unexpected ARM page type.");

    }
}

स्थिर create_mappings_pde_return_t
createSafeMappingEntries_PDE
(paddr_t आधारः, word_t vaddr, vm_page_आकार_t frame_आकार,
 vm_rights_t vmRights, vm_गुणाः_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_चिह्नकम्_t currentPDEType;
    word_t i;

    विकल्प (frame_आकार) {

    /* PDE mappings */
    पक्ष ARMSection:
        ret.pde_entries.आधारः = lookupPDSlot(pd, vaddr);
        ret.pde_entries.दीर्घता = 1;

        ret.pde = makeUserPDE(ARMSection, आधारः,
                              vm_गुणाः_get_arm_Parity_Enabled(attr),
                              vm_गुणाः_get_arm_Page_Cacheable(attr),
                              vm_गुणाः_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.आधारः);
        यदि (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_आकार(ret.pde_entries.आधारः) != 0))) {
#अन्यथा
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.आधारः) != 0))) {
#यदि_अन्तः
            current_syscall_error.type =
                seL4_DeleteFirst;
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;

            प्रत्यागम ret;
        }

        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    पक्ष ARMSuperSection:
        ret.pde_entries.आधारः = lookupPDSlot(pd, vaddr);
        ret.pde_entries.दीर्घता = SECTIONS_प्रत्येकम्_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, आधारः,
                              vm_गुणाः_get_arm_Parity_Enabled(attr),
                              vm_गुणाः_get_arm_Page_Cacheable(attr),
                              vm_गुणाः_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        प्रति (i = 0; i < SECTIONS_प्रत्येकम्_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.आधारः[i]);
            यदि (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_आकार(ret.pde_entries.आधारः[i]) != 1))) {
#अन्यथा
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.आधारः[i]) != 1))) {
#यदि_अन्तः
                current_syscall_error.type =
                    seL4_DeleteFirst;
                ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;

                प्रत्यागम ret;
            }
        }

        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    स्वभाव:
        fail("Invalid or unexpected ARM page type.");

    }
}

स्थिर अन्तःस्थ vptr_t page_आधारः(vptr_t vaddr, vm_page_आकार_t आकार)
{
    प्रत्यागम vaddr & ~आवरणम्(page_द्वयङ्काः_For_आकार(आकार));
}

स्थिर bool_t विशुद्धम् pteCheckIfMapped(pte_t *pte)
{
    प्रत्यागम pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

स्थिर bool_t विशुद्धम् pdeCheckIfMapped(pde_t *pde)
{
    प्रत्यागम pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

स्थिर शून्य doFlush(पूर्णाङ्क invLabel, vptr_t start, vptr_t अन्तः_2, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        अन्तः_2 = (vptr_t)paddr_गन्तव्यम्_pptr(pstart) + (अन्तः_2 - start);
        start = (vptr_t)paddr_गन्तव्यम्_pptr(pstart);
    }
    विकल्प (invLabel) {
    पक्ष ARMPDClean_दत्त:
    पक्ष ARMPage_Clean_दत्त:
        cleanCacheRange_RAM(start, अन्तः_2, pstart);
        भङ्ग;
    पक्ष ARMPDInvalidate_दत्त:
    पक्ष ARMPage_Invalidate_दत्त:
        invalidateCacheRange_RAM(start, अन्तः_2, pstart);
        भङ्ग;
    पक्ष ARMPDClean_Invalidate_दत्त:
    पक्ष ARMPage_Clean_Invalidate_दत्त:
        cleanInvalidateCacheRange_RAM(start, अन्तः_2, pstart);
        भङ्ग;
    पक्ष ARMPDUnify_Instruction:
    पक्ष ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(start, अन्तः_2, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(start, अन्तः_2, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(start, अन्तः_2, pstart);
        /* ... then wait for the completion of invalidations. */
        dsb();
        /* Ensure new instructions come from fresh cache lines. */
        isb();
        भङ्ग;
    स्वभाव:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

स्थिर exception_t performPDFlush(पूर्णाङ्क invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                  vptr_t अन्तः_2, paddr_t pstart)
{
    bool_t मूलम्_switched;

    /* Flush if given a non zero range */
    यदि (start < अन्तः_2) {
        मूलम्_switched = समुच्चयः_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, start, अन्तः_2, pstart);

        यदि (मूलम्_switched) {
            समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_Page_सारणी_Invocation_Map(cap_t cap, cte_t *ctSlot,
                                                 pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_Page_सारणी_Invocation_Unmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));
        unmap_Page_सारणी(
            cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap),
            cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
            pt);
        clear_स्मृतिः_PT((शून्य *)pt, cap_get_cap_आकार_द्वयङ्काः(cap));
    }
    cap_page_सारणी_cap_ptr_समुच्चयः_cap_PTIs_Mapped(&(ctSlot->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageInvocationMapPTE(asid_t asid, cap_t cap, cte_t *ctSlot, pte_t pte,
                                               pte_range_t pte_entries)
{
    word_t i, j अप्रयुक्तम्;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.आधारः);

    j = pte_entries.दीर्घता;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t आधारः_स्थानाङ्कः = pte_pte_small_get_स्थानाङ्कः(pte);
#यदि_अन्तः
    प्रति (i = 0; i < pte_entries.दीर्घता; i++) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_समुच्चयः_स्थानाङ्कः(pte, आधारः_स्थानाङ्कः + i * BIT(page_द्वयङ्काः_For_आकार(ARMSmallPage)));
#यदि_अन्तः
        pte_entries.आधारः[i] = pte;
    }
    cleanCacheRange_PoU((word_t)pte_entries.आधारः,
                        चरमः_BYTE_PTE(pte_entries.आधारः, pte_entries.दीर्घता),
                        addrFromPPtr(pte_entries.आधारः));
    यदि (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageInvocationMapPDE(asid_t asid, cap_t cap, cte_t *ctSlot, pde_t pde,
                                               pde_range_t pde_entries)
{
    word_t i, j अप्रयुक्तम्;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.आधारः);

    j = pde_entries.दीर्घता;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t आधारः_स्थानाङ्कः = pde_pde_section_get_स्थानाङ्कः(pde);
#यदि_अन्तः
    प्रति (i = 0; i < pde_entries.दीर्घता; i++) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_समुच्चयः_स्थानाङ्कः(pde, आधारः_स्थानाङ्कः + i * BIT(page_द्वयङ्काः_For_आकार(ARMSection)));
#यदि_अन्तः
        pde_entries.आधारः[i] = pde;
    }
    cleanCacheRange_PoU((word_t)pde_entries.आधारः,
                        चरमः_BYTE_PDE(pde_entries.आधारः, pde_entries.दीर्घता),
                        addrFromPPtr(pde_entries.आधारः));
    यदि (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    यदि (generic_frame_cap_get_capFIsMapped(cap)) {
        unmapPage(generic_frame_cap_get_capFSize(cap),
                  generic_frame_cap_get_capFMappedASID(cap),
                  generic_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                  (शून्य *)generic_frame_cap_get_capFBasePtr(cap));
    }

    generic_frame_cap_ptr_समुच्चयः_cap_FMapped_स्थानाङ्कः(&ctSlot->cap, asidInvalid, 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageFlush(पूर्णाङ्क invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                    vptr_t अन्तः_2, paddr_t pstart)
{
    bool_t मूलम्_switched;

    /* now we can flush. But only if we were given a non zero range */
    यदि (start < अन्तः_2) {
        मूलम्_switched = समुच्चयः_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, start, अन्तः_2, pstart);

        यदि (मूलम्_switched) {
            समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_Page_Get_स्थानाङ्कः(शून्य *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    यदि (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, ipc_बफर, 0, capFBasePtr);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pdCapSlot)
{
    cap_page_directory_cap_ptr_समुच्चयः_cap_PDMapped_ASID(&pdCapSlot->cap, asid);
    cap_page_directory_cap_ptr_समुच्चयः_cap_PDIs_Mapped(&pdCapSlot->cap, 1);
    poolPtr->array[ASID_नीचः(asid)] =
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCapSlot->cap));

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performASIDControlInvocation(शून्य *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_आधारः)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_समुच्चयः_cap_Free_सूचकांक(&(parent->cap),
                                         MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_खण्डम्_आकार(parent->cap)));

    memzero(frame, 1 << ARMSmall_Page_द्वयङ्काः);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_निवेशय(cap_asid_pool_cap_new(asid_आधारः, WORD_REF(frame)),
              parent, slot);;
    /* Haskell error: "ASID pool's base must be aligned" */
    प्रतिज्ञा((asid_आधारः & आवरणम्(asid_नीचः_द्वयङ्काः)) == 0);
    armKSASIDTable[ASID_उच्चः(asid_आधारः)] = (asid_pool_t *)frame;

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeARMPageDirectoryInvocation(word_t invLabel, word_t दीर्घता,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    word_t *बफर)
{
    विकल्प (invLabel) {
    पक्ष ARMPDClean_दत्त:
    पक्ष ARMPDInvalidate_दत्त:
    पक्ष ARMPDClean_Invalidate_दत्त:
    पक्ष ARMPDUnify_Instruction: {
        vptr_t start, अन्तः_2;
        paddr_t pstart;
        findPDForASID_ret_t find_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        यदि (दीर्घता < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        अन्तः_2 =   getSyscallArg(1, बफर);

        /* Check sanity of arguments */
        यदि (अन्तः_2 <= start) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        यदि (start >= USER_शिखरम् || अन्तः_2 > USER_शिखरम्) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(cap_get_capType(cap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        asid = cap_page_directory_cap_get_capPDMappedASID(cap);

        find_ret = findPDForASID(asid);
        यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(find_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, start);

        /* Check that there's actually something there. */
        यदि (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
            प्रत्यागम EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        यदि (page_आधारः(start, resolve_ret.frame_आकार) !=
            page_आधारः(अन्तः_2 - 1, resolve_ret.frame_आकार)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax =
                page_आधारः(start, resolve_ret.frame_आकार) +
                आवरणम्(page_द्वयङ्काः_For_आकार(resolve_ret.frame_आकार));
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.frame_आधारः
                 + (start & आवरणम्(page_द्वयङ्काः_For_आकार(resolve_ret.frame_आकार)));

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        यदि (pstart < phys_आधारः() || pstart > PADDR_शिखरम् || (अन्तः_2 - start) > PADDR_शिखरम् - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPDFlush(invLabel, pd, asid, start, अन्तः_2 - 1, pstart);
    }

    स्वभाव:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

}

स्थिर exception_t decode_ARMPage_सारणी_Invocation(word_t invLabel, word_t दीर्घता,
                                                cte_t *cte, cap_t cap, word_t *बफर)
{
    word_t vaddr, pd_सूचकांक;

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_गुणाः_t attr;
#यदि_अन्तः
    cap_t pdCap;
    pde_t *pd, *pdSlot;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    यदि (invLabel == ARMPage_सारणी_Unmap) {
        यदि (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Page_सारणी_Invocation_Unmap(cap, cte);
    }

    यदि (unlikely(invLabel != ARMPage_सारणी_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_गुणाः_From_Word(getSyscallArg(1, बफर));
#यदि_अन्तः
    pdCap = current_extra_caps.excaprefs[0]->cap;

    यदि (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                 !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    यदि (unlikely(vaddr >= USER_शिखरम्)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_शिखरम्);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    {
        findPDForASID_ret_t find_ret;

        find_ret = findPDForASID(asid);
        यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(find_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_सूचकांक = vaddr >> (PAGE_द्वयङ्काः + PT_सूचकांक_द्वयङ्काः);
    pdSlot = &pd[pd_सूचकांक];
    यदि (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = seL4_DeleteFirst;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap)));
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_गुणाः_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#अन्यथा
    pde = pde_pde_coarse_new(paddr);
#यदि_अन्तः

    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTIs_Mapped(cap, 1);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_ASID(cap, asid);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_स्थानाङ्कः(cap, vaddr);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम perform_Page_सारणी_Invocation_Map(cap, cte, pde, pdSlot);
}

स्थिर exception_t decodeARMFrameInvocation(word_t invLabel, word_t दीर्घता,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    विकल्प (invLabel) {
    पक्ष ARMPageMap: {
        word_t vaddr, vtop, w_rights_आवरणम्;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_आकार_t frame_आकार;
        vm_गुणाः_t attr;

        यदि (unlikely(दीर्घता < 3 || current_extra_caps.excaprefs[0] == रिक्त)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_सन्देशः;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, बफर);
        w_rights_आवरणम् = getSyscallArg(1, बफर);
        attr = vm_गुणाः_From_Word(getSyscallArg(2, बफर));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        frame_आकार = generic_frame_cap_get_capFSize(cap);
        capVMRights = generic_frame_cap_get_capFVMRights(cap);

        यदि (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));
        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        यदि (generic_frame_cap_get_capFIsMapped(cap)) {
            यदि (generic_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_सङ्ख्या = 1;

                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }

            यदि (generic_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_तर्कः;
                current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;

                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            vtop = vaddr + BIT(page_द्वयङ्काः_For_आकार(frame_आकार)) - 1;

            यदि (unlikely(vtop >= USER_शिखरम्)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_शिखरम्);
                current_syscall_error.type =
                    se_L4_Invalid_तर्कः;
                current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;

                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findPDForASID_ret_t find_ret;

            find_ret = findPDForASID(asid);
            यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;

                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }

            यदि (unlikely(find_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_सङ्ख्या = 1;

                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            आवरणम्_VMRights(capVMRights, rightsFromWord(w_rights_आवरणम्));

        यदि (unlikely(!checkVPAlignment(frame_आकार, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((शून्य *)
                                   generic_frame_cap_get_capFBasePtr(cap));

        cap = generic_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap, asid,
                                                      vaddr);
        यदि (frame_आकार == ARMSmallPage || frame_आकार == ARMLargePage) {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(capFBasePtr, vaddr,
                                                   frame_आकार, vmRights,
                                                   attr, pd);
            यदि (unlikely(map_ret.स्थितिः != EXCEPTION_NONE)) {
#यदि_परिभाषितम् CONFIG_PRINTING
                यदि (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#यदि_अन्तः
                प्रत्यागम map_ret.स्थितिः;
            }

            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
            प्रत्यागम performPageInvocationMapPTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } अन्यथा {
            create_mappings_pde_return_t map_ret;
            map_ret = createSafeMappingEntries_PDE(capFBasePtr, vaddr,
                                                   frame_आकार, vmRights,
                                                   attr, pd);
            यदि (unlikely(map_ret.स्थितिः != EXCEPTION_NONE)) {
#यदि_परिभाषितम् CONFIG_PRINTING
                यदि (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#यदि_अन्तः
                प्रत्यागम map_ret.स्थितिः;
            }

            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
            प्रत्यागम performPageInvocationMapPDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    पक्ष ARMPageUnmap: {
#यदि_परिभाषितम् CONFIG_TK1_SMMU
        यदि (isIOSpaceFrameCap(cap)) {
            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
            प्रत्यागम performPageInvocationUnmapIO(cap, cte);
        } अन्यथा
#यदि_अन्तः
        {
            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
            प्रत्यागम performPageInvocationUnmap(cap, cte);
        }
    }

#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष ARMPageMapIO: {
        प्रत्यागम decodeARMIOMapInvocation(invLabel, दीर्घता, cte, cap, बफर);
    }
#यदि_अन्तः

    पक्ष ARMPage_Clean_दत्त:
    पक्ष ARMPage_Invalidate_दत्त:
    पक्ष ARMPage_Clean_Invalidate_दत्त:
    पक्ष ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        findPDForASID_ret_t pd;
        vptr_t start, अन्तः_2;
        paddr_t pstart;
        word_t page_आकार;
        word_t page_आधारः_2;

        यदि (दीर्घता < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_frame_cap_get_capFMappedASID(cap);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_frame_cap_get_capFBasePtr(cap);
#अन्यथा
        vaddr = generic_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
#यदि_अन्तः

        यदि (unlikely(!generic_frame_cap_get_capFIsMapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pd = findPDForASID(asid);
        यदि (unlikely(pd.स्थितिः != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        अन्तः_2 =   getSyscallArg(1, बफर);

        /* check that the range is sane */
        यदि (अन्तः_2 <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        page_आकार = 1 << page_द्वयङ्काः_For_आकार(generic_frame_cap_get_capFSize(cap));
        page_आधारः_2 = addrFromPPtr((शून्य *)generic_frame_cap_get_capFBasePtr(cap));

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        यदि (unlikely(!resolve_ret.valid ||
                     resolve_ret.frame_आकार != generic_frame_cap_get_capFSize(cap) ||
                     resolve_ret.frame_आधारः != page_आधारः_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः

        यदि (start >= page_आकार || अन्तः_2 > page_आकार) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = page_आधारः_2 + start;
        start += vaddr;
        अन्तः_2 += vaddr;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        यदि (pstart < phys_आधारः() || pstart > PADDR_शिखरम् || (अन्तः_2 - start) > PADDR_शिखरम् - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPageFlush(invLabel, pd.pd, asid, start, अन्तः_2 - 1, pstart);
    }

    पक्ष ARMPage_Get_स्थानाङ्कः: {


        /* Check that there are enough message registers */
        प्रतिज्ञा(n_सन्देशः_Registers >= 1);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Page_Get_स्थानाङ्कः((शून्य *)generic_frame_cap_get_capFBasePtr(cap), call);
    }

    स्वभाव:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t दीर्घता, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_directory_cap:
        प्रत्यागम decodeARMPageDirectoryInvocation(invLabel, दीर्घता, cptr, cte,
                                                cap, बफर);

    पक्ष cap_page_सारणी_cap:
        प्रत्यागम decode_ARMPage_सारणी_Invocation(invLabel, दीर्घता, cte, cap, बफर);

    पक्ष cap_small_frame_cap:
    पक्ष cap_frame_cap:
        प्रत्यागम decodeARMFrameInvocation(invLabel, दीर्घता, cte, cap, call, बफर);

    पक्ष cap_asid_control_cap: {
        word_t i;
        asid_t asid_आधारः;
        word_t सूचकांक, गभीरता;
        cap_t untyped, मूलम्;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        शून्य *frame;
        exception_t स्थितिः;

        यदि (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त
                     || current_extra_caps.excaprefs[1] == रिक्त)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        सूचकांक = getSyscallArg(0, बफर);
        गभीरता = getSyscallArg(1, बफर);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        मूलम् = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        प्रति (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        यदि (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid_आधारः = i << asid_नीचः_द्वयङ्काः;

        यदि (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_खण्डम्_आकार(untyped) !=
                     se_L4_ASIDPool_द्वयङ्काः || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        स्थितिः = ensureNoChildren(parentSlot);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            प्रत्यागम स्थितिः;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(मूलम्, सूचकांक, गभीरता);
        यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            प्रत्यागम lu_ret.स्थितिः;
        }
        destSlot = lu_ret.slot;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performASIDControlInvocation(frame, destSlot,
                                            parentSlot, asid_आधारः);
    }

    पक्ष cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pdCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        यदि (unlikely(invLabel != ARMASIDPool_नियोजय)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pdCapSlot = current_extra_caps.excaprefs[0];
        pdCap = pdCapSlot->cap;

        यदि (unlikely(
                cap_get_capType(pdCap) != cap_page_directory_cap ||
                cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_उच्चः(cap_asid_pool_cap_get_capASIDBase(cap))];
        यदि (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_मूलम्_new();

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        प्रति (i = 0; i < (1 << asid_नीचः_द्वयङ्काः) && (asid + i == 0 || pool->array[i]); i++);

        यदि (unlikely(i == 1 << asid_नीचः_द्वयङ्काः)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performASIDPoolInvocation(asid, pool, pdCapSlot);
    }

    स्वभाव:
        fail("Invalid ARM arch cap type");
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
exception_t benchmark_arch_map_log_बफर(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_आकार_t frame_आकार;
    pptr_t  frame_pptr;

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

    frame_आकार = generic_frame_cap_get_capFSize(lu_ret.cap);

    यदि (frame_आकार != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = generic_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_गन्तव्यम्_paddr((शून्य *) frame_pptr);

    प्रति (पूर्णाङ्क idx = 0; idx < BIT(PT_सूचकांक_द्वयङ्काः); idx++) {
        paddr_t physical_स्थानाङ्कः = ks_User_Log_बफर + (idx << se_L4_Page_द्वयङ्काः);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_स्थानाङ्कः,
                0, /* global */
                0, /* Not shared */
                0, /* APX = 0, privileged full access */
                0, /* TEX = 0 */
                1, /* VMKernelOnly */
                1, /* Cacheable */
                0, /* Write-through to minimise perf hit */
                0  /* executable */
            );

        cleanByVA_PoU((vptr_t)&armKSGlobalLogPT[idx], addrFromKPPtr(&armKSGlobalLogPT[idx]));
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * BIT(se_L4_Page_द्वयङ्काः)));
    }

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
शून्य kernelPrefetchAbort(word_t pc, word_t lr) VISIBLE;
शून्य kernel_दत्त_Abort(word_t pc) VISIBLE;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

शून्य kernelUndefinedInstruction(word_t pc) VISIBLE;

शून्य kernelPrefetchAbort(word_t pc, word_t lr)
{
    प्रारूप_लिख("\n\nKERNEL PREFETCH ABORT!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    प्रारूप_लिख("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

शून्य kernel_दत्त_Abort(word_t pc)
{
    प्रारूप_लिख("\n\nKERNEL DATA ABORT!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("HDFAR: 0x%"SEL4_PRIx_word" HSR: 0x%"SEL4_PRIx_word"\n",
           getHDFAR(), getHSR());
    halt();
}

शून्य kernelUndefinedInstruction(word_t pc)
{
    प्रारूप_लिख("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    halt();
}

#अन्यथा /* CONFIG_ARM_HYPERVISOR_SUPPORT */

शून्य kernelPrefetchAbort(word_t pc, word_t lr)
{
    प्रारूप_लिख("\n\nKERNEL PREFETCH ABORT!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("IFSR: 0x%"SEL4_PRIx_word"\n", getIFSR());
    प्रारूप_लिख("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

शून्य kernel_दत्त_Abort(word_t pc)
{
    प्रारूप_लिख("\n\nKERNEL DATA ABORT!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("FAR: 0x%"SEL4_PRIx_word" DFSR: 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_PRINTING
प्रकारनिर्देश संरचना readWordFromVSpace_ret {
    exception_t स्थितिः;
    word_t मानम्;
} readWordFromVSpace_ret_t;

स्थिर readWordFromVSpace_ret_t readWordFromVSpace(pde_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    pde_t *pdSlot;
    paddr_t paddr;
    word_t विस्थापनम्;
    pptr_t kernel_vaddr;
    word_t *मानम्;

    pdSlot = lookupPDSlot(pd, vaddr);
    यदि (pde_ptr_get_pdeType(pdSlot) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_स्थानाङ्कः(pdSlot);
        विस्थापनम् = vaddr & आवरणम्(ARMSection_द्वयङ्काः);
    } अन्यथा {
        ptSlot = lookupPTSlot(pd, vaddr);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (ptSlot.स्थितिः == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_स्थानाङ्कः(ptSlot.ptSlot);
            यदि (pte_pte_small_ptr_get_contiguous_hint(ptSlot.ptSlot)) {
                विस्थापनम् = vaddr & आवरणम्(ARMLarge_Page_द्वयङ्काः);
            } अन्यथा {
                विस्थापनम् = vaddr & आवरणम्(ARMSmall_Page_द्वयङ्काः);
            }
#अन्यथा
        यदि (ptSlot.स्थितिः == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_स्थानाङ्कः(ptSlot.ptSlot);
            विस्थापनम् = vaddr & आवरणम्(ARMSmall_Page_द्वयङ्काः);
        } अन्यथा यदि (ptSlot.स्थितिः == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_स्थानाङ्कः(ptSlot.ptSlot);
            विस्थापनम् = vaddr & आवरणम्(ARMLarge_Page_द्वयङ्काः);
#यदि_अन्तः
        } अन्यथा {
            ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
            प्रत्यागम ret;
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
    pde_t *pd;
    word_t sp;
    पूर्णाङ्क i;

    thread_मूलम् = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    यदि (cap_get_capType(thread_मूलम्) != cap_page_directory_cap) {
        प्रारूप_लिख("Invalid vspace\n");
        प्रत्यागम;
    }

    pd = (pde_t *)pptr_सम्बन्धि_cap(thread_मूलम्);

    sp = get_पञ्जिकाकोशः(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    यदि (!IS_ALIGNED(sp, se_L4_Word_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("SP not aligned\n");
        प्रत्यागम;
    }

    प्रति (i = 0; i < CONFIG_USER_राशिः_TRACE_दीर्घता; i++) {
        word_t स्थानाङ्कः = sp + (i * परिमाण(word_t));
        readWordFromVSpace_ret_t परिणाम;
        परिणाम = readWordFromVSpace(pd, स्थानाङ्कः);
        यदि (परिणाम.स्थितिः == EXCEPTION_NONE) {
            प्रारूप_लिख("0x%lx: 0x%lx\n", (दीर्घ)स्थानाङ्कः, (दीर्घ)परिणाम.मानम्);
        } अन्यथा {
            प्रारूप_लिख("0x%lx: INVALID\n", (दीर्घ)स्थानाङ्कः);
        }
    }
}
#यदि_अन्तः
