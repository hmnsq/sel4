/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/stack.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <machine/debug.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/untyped.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <armv/context_switch.h>
#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <arch/machine/tlb.h>

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#అన్తర్భావ <benchmark/benchmark_track.h>
#అయితే_ముగింపు

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#నిర్వచించు SECTIONS_చొప్పున_SUPER_SECTION బిట్టు(ARMSuper_Section_బిట్లు - ARMSection_బిట్లు)
#నిర్వచించు పుటలు_చొప్పున_LARGE_పుట బిట్టు(ARMLarge_పుట_బిట్లు - ARMSmall_పుట_బిట్లు)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#నిర్వచించు చివరి_BYTE_PTE(PTE,నిడివి_2) ((మాట_t)&(PTE)[(నిడివి_2)-1] + (బిట్టు(PTE_కొలత_బిట్లు)-1))
#నిర్వచించు చివరి_BYTE_PDE(PDE,నిడివి_2) ((మాట_t)&(PDE)[(నిడివి_2)-1] + (బిట్టు(PDE_కొలత_బిట్లు)-1))

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#నిర్వచించు MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#నిర్వచించు MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#నిర్వచించు ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#నిర్వచించు ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */

స్థిర శూన్య సమితి_Global_PD(శూన్య);

నిర్మాణం resolve_ret {
    paddr_t చట్రం_పునాది;
    vm_పుట_కొలత_t చట్రం_కొలత;
    bool_t valid;
};
రకనిర్వచనం నిర్మాణం resolve_ret resolve_ret_t;

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
స్థిర bool_t శుద్ధ pteCheckIfMapped(pte_t *pte);
స్థిర bool_t శుద్ధ pdeCheckIfMapped(pde_t *pde);

స్థిర మాట_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ఎంపిక (vm_rights) {
    స్థితి VMNoAccess:
        తిరిగి 0;

    స్థితి VMKernelOnly:
        తిరిగి 1;

    స్థితి VMReadOnly:
        తిరిగి 2;

    స్థితి VMReadWrite:
        తిరిగి 3;

    అప్రమేయ:
        fail("Invalid VM rights");
    }
}

#లేకపోతే
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_సంకేతం
స్థిర మాట_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ఎంపిక (vm_rights) {
    స్థితి VMKernelOnly:
        తిరిగి 0;
    స్థితి VMReadWrite:
        తిరిగి 1;
    స్థితి VMNoAccess:
        /* RO at PL1 only */
        తిరిగి 2;
    స్థితి VMReadOnly:
        తిరిగి 3;
    అప్రమేయ:
        fail("Invalid VM rights");
    }
}

స్థిర మాట_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    ఎంపిక (vm_rights) {
    స్థితి VMKernelOnly:
    స్థితి VMNoAccess:
        తిరిగి 0;
    స్థితి VMReadOnly:
        తిరిగి 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    స్థితి VMReadWrite:
        తిరిగి 3;
    అప్రమేయ:
        fail("Invalid VM rights");
    }
}

#అయితే_ముగింపు

vm_rights_t CONST ముసుగు_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ముసుగు)
{
    ఒకవేళ (vm_rights == VMNoAccess) {
        తిరిగి VMNoAccess;
    }
    ఒకవేళ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        తిరిగి VMReadOnly;
    }
    ఒకవేళ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        ఒకవేళ (!seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
            తిరిగి VMReadOnly;
        } లేకపోతే {
            తిరిగి VMReadWrite;
        }
    }
    ఒకవేళ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
        userError("Attempted to make unsupported write only mapping");
    }
    తిరిగి VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_సంకేతం శూన్య map_kernel_చట్రం(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_లక్షణాలు_t లక్షణాలు)
{
    మాట_t idx = (vaddr & ముసుగు(పుట_బిట్లు_For_కొలత(ARMSection))) >> పుట_బిట్లు_For_కొలత(ARMSmall_పుట);

    ప్రతిజ్ఞా(vaddr >= PPTR_శిఖరం); /* vaddr lies in the region the global PT covers */
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t tex;
    ఒకవేళ (vm_లక్షణాలు_get_arm_పుట_Cacheable(లక్షణాలు)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } లేకపోతే {
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
#లేకపోతే /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
            vm_లక్షణాలు_get_arm_పుట_Cacheable(లక్షణాలు)
        );
#అయితే_ముగింపు /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_సంకేతం శూన్య map_kernel_కిటికీ(శూన్య)
{
    paddr_t  phys;
    మాట_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_పునాది();
    idx = PPTR_పునాది >> పుట_బిట్లు_For_కొలత(ARMSection);

    అయితే (idx < బిట్టు(PD_సూచీ_బిట్లు) - SECTIONS_చొప్పున_SUPER_SECTION) {
        మాట_t idx2;

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
        కొరకు (idx2 = idx; idx2 < idx + SECTIONS_చొప్పున_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += బిట్టు(పుట_బిట్లు_For_కొలత(ARMSuperSection));
        idx += SECTIONS_చొప్పున_SUPER_SECTION;
    }

    అయితే (idx < (PPTR_శిఖరం >> పుట_బిట్లు_For_కొలత(ARMSection))) {
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
        phys += బిట్టు(పుట_బిట్లు_For_కొలత(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ప్రతిజ్ఞా(phys == PADDR_శిఖరం);

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*చిరునామా*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

    phys += బిట్టు(పుట_బిట్లు_For_కొలత(ARMSection));
    idx++;
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*చిరునామా*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_పుట_పట్టిక_బిట్లు);

    /* map vector table */
    map_kernel_చట్రం(
        addrFromKPPtr(arm_సదిశ_పట్టిక),
        PPTR_సదిశ_పట్టిక,
        VMKernelOnly,
        vm_లక్షణాలు_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#లేకపోతే /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_సంకేతం శూన్య map_kernel_కిటికీ(శూన్య)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t వాడని pteS2;

    /* Initialise PGD */
    కొరకు (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    కొరకు (idx = 0; idx < (USER_శిఖరం - 0xC0000000) >> (PT_సూచీ_బిట్లు + పుట_బిట్లు); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_పునాది();
    కొరకు (; idx < బిట్టు(PT_సూచీ_బిట్లు) - 1; idx++) {
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
        phys += బిట్టు(PT_సూచీ_బిట్లు + పుట_బిట్లు);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_పుట_పట్టిక_బిట్లు);
    కొరకు (idx = 0; idx < 256; idx++) {
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
        phys += బిట్టు(పుట_బిట్లు);
    }
    /* map vector table */
    map_kernel_చట్రం(
        addrFromKPPtr(arm_సదిశ_పట్టిక),
        PPTR_సదిశ_పట్టిక,
        VMKernelOnly,
        vm_లక్షణాలు_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#అయితే_ముగింపు /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

స్థిర BOOT_సంకేతం శూన్య map_it_చట్రం_cap(cap_t pd_cap, cap_t చట్రం_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *లక్ష్యం_స్థానం;
    pde_t *pd    = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    శూన్య  *చట్రం = (శూన్య *)generic_చట్రం_cap_get_cap_FBase_Ptr(చట్రం_cap);
    vptr_t vptr  = generic_చట్రం_cap_get_cap_FMapped_చిరునామా(చట్రం_cap);

    ప్రతిజ్ఞా(generic_చట్రం_cap_get_cap_FMapped_ASID(చట్రం_cap) != 0);

    pd += (vptr >> పుట_బిట్లు_For_కొలత(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_చిరునామా(pd));
    లక్ష్యం_స్థానం = pt + ((vptr & ముసుగు(పుట_బిట్లు_For_కొలత(ARMSection)))
                       >> పుట_బిట్లు_For_కొలత(ARMSmall_పుట));
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    *లక్ష్యం_స్థానం = pte_pte_small_new(
                      addrFromPPtr(చట్రం),
                      1, /* not global */
                      SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                      0, /* APX = 0, privileged full access */
                      5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                      APFromVMRights(VMReadWrite),
                      0, /* C (Inner write allocate) */
                      1, /* B (Inner write allocate) */
                      !executable
                  );
#లేకపోతే
    *లక్ష్యం_స్థానం = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(చట్రం),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#అయితే_ముగింపు
}

/* Create a frame cap for the initial thread. */

స్థిర BOOT_సంకేతం cap_t create_it_చట్రం_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    ఒకవేళ (use_large)
        తిరిగి
            cap_చట్రం_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_దిగువ(asid),                /* capFMappedASIDLow  */
                మాట_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_ఎగువ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    లేకపోతే
        తిరిగి
            cap_small_చట్రం_cap_new(
                ASID_దిగువ(asid),                /* capFMappedASIDLow  */
                మాట_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#నిర్వచితమైతే CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#అయితే_ముగింపు
                ASID_ఎగువ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

స్థిర BOOT_సంకేతం శూన్య map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(pt_cap);
    pde_t *లక్ష్యం_స్థానం = pd + (vptr >> పుట_బిట్లు_For_కొలత(ARMSection));

    ప్రతిజ్ఞా(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(pt_cap));

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    *లక్ష్యం_స్థానం = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*చిరునామా*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#లేకపోతే
    *లక్ష్యం_స్థానం = pde_pde_coarse_new(addrFromPPtr(pt));
#అయితే_ముగింపు
}

/* Create a page table for the initial thread */

స్థిర BOOT_సంకేతం cap_t create_it_పుట_పట్టిక_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ఒకవేళ (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    తిరిగి cap;
}

BOOT_సంకేతం మాట_t arch_get_n_paging(v_region_t it_v_reg)
{
    తిరిగి get_n_paging(it_v_reg, PT_సూచీ_బిట్లు + పుట_బిట్లు);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_సంకేతం cap_t create_it_చిరునామా_space(cap_t మూలం_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_స్థానం_Pos స్థానం_pos_before;
    se_L4_స్థానం_Pos స్థానం_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_పుట_Dir_బిట్లు) - 1,
                        addrFromPPtr((శూన్య *)rootserver.vspace));
    cap_t pd_cap =
        cap_పుట_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    కొరకు (pt_vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, PT_సూచీ_బిట్లు + పుట_బిట్లు);
         pt_vptr < it_v_reg.చివర;
         pt_vptr += బిట్టు(PT_సూచీ_బిట్లు + పుట_బిట్లు)) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap,
                         create_it_పుట_పట్టిక_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            తిరిగి cap_null_cap_new();
        }
    }

    స్థానం_pos_after = ndks_boot.స్థానం_pos_cur;
    ndks_boot.bi_చట్రం->userImagePaging = (se_L4_స్థానం_Region) {
        స్థానం_pos_before, స్థానం_pos_after
    };

    తిరిగి pd_cap;
}

BOOT_సంకేతం cap_t create_unmapped_it_చట్రం_cap(pptr_t pptr, bool_t use_large)
{
    తిరిగి create_it_చట్రం_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_సంకేతం cap_t create_mapped_it_చట్రం_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_చట్రం_cap(pptr, vptr, asid, use_large);
    map_it_చట్రం_cap(pd_cap, cap, executable);
    తిరిగి cap;
}

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_సంకేతం శూన్య activate_kernel_vspace(శూన్య)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    సమితి_Global_PD();
    invalidateLocalTLB();
    తాళం_TLBEntry(PPTR_పునాది);
    తాళం_TLBEntry(PPTR_సదిశ_పట్టిక);
}

#లేకపోతే

BOOT_సంకేతం శూన్య activate_kernel_vspace(శూన్య)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    సమితి_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    సమితి_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#ఒకవేళ 0 /* Can't lock entries on A15 */
    తాళం_TLBEntry(PPTR_పునాది);
    తాళం_TLBEntry(PPTR_సదిశ_పట్టిక);
#అయితే_ముగింపు
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= బిట్టు(8);       /* Inner write-back, write-allocate */
    r |= బిట్టు(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= బిట్టు(31);
    సమితి_VTCR(r);
}

#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_సంకేతం శూన్య write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_యొక్క_cap(it_ap_cap));
    ap->array[ASID_దిగువ(IT_ASID)] = PDE_PTR(pptr_యొక్క_cap(it_pd_cap));
    armKSASIDTable[ASID_ఎగువ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

కనుగొను_PDFor_ASID_ret_t కనుగొను_PDFor_ASID(asid_t asid)
{
    కనుగొను_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.pd = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    pd = poolPtr->array[ASID_దిగువ(asid)];
    ఒకవేళ (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.pd = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ret.pd = pd;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

మాట_t *శుద్ధ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    మాట_t w_బఫరు_Ptr;
    cap_t బఫరు_Cap;
    vm_rights_t vm_rights;

    w_బఫరు_Ptr = thread->tcbIPCBuffer;
    బఫరు_Cap = TCB_PTR_CTE_PTR(thread, tcb_బఫరు)->cap;

    ఒకవేళ (unlikely(cap_get_capType(బఫరు_Cap) != cap_small_చట్రం_cap &&
                 cap_get_capType(బఫరు_Cap) != cap_చట్రం_cap)) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (unlikely(generic_చట్రం_cap_get_cap_FIs_Device(బఫరు_Cap))) {
        తిరిగి ఖాళీ;
    }

    vm_rights = generic_చట్రం_cap_get_cap_FVMRights(బఫరు_Cap);
    ఒకవేళ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        మాట_t పునాది_Ptr;
        గుర్తులేని పూర్ణాంకం పుట_బిట్లు_2;

        పునాది_Ptr = generic_చట్రం_cap_get_cap_FBase_Ptr(బఫరు_Cap);
        పుట_బిట్లు_2 = పుట_బిట్లు_For_కొలత(generic_చట్రం_cap_get_cap_FSize(బఫరు_Cap));
        తిరిగి (మాట_t *)(పునాది_Ptr + (w_బఫరు_Ptr & ముసుగు(పుట_బిట్లు_2)));
    } లేకపోతే {
        తిరిగి ఖాళీ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ఒకవేళ (unlikely(cap_get_capType(cap) != cap_small_చట్రం_cap &&
                 cap_get_capType(cap) != cap_చట్రం_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (unlikely(generic_చట్రం_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(vptr & ముసుగు(se_L4_IPCBuffer_కొలత_బిట్లు))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (పూర్ణాంకం)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    గుర్తులేని పూర్ణాంకం pd_సూచీ;

    pd_సూచీ = vptr >> (పుట_బిట్లు + PT_సూచీ_బిట్లు);
    తిరిగి pd + pd_సూచీ;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_స్థానం;

    pd_స్థానం = lookupPDSlot(pd, vptr);

    ఒకవేళ (unlikely(pde_ptr_get_pdeType(pd_స్థానం) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_సూచీ_బిట్లు + పుట_బిట్లు);

        ret.pt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    } లేకపోతే {
        pte_t *pt, *pt_స్థానం;
        గుర్తులేని పూర్ణాంకం pt_సూచీ;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_చిరునామా(pd_స్థానం));
        pt_సూచీ = (vptr >> పుట_బిట్లు) & ముసుగు(PT_సూచీ_బిట్లు);
        pt_స్థానం = pt + pt_సూచీ;

        ret.pt_స్థానం = pt_స్థానం;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    }
}

స్థిర pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    గుర్తులేని పూర్ణాంకం pt_సూచీ;

    pt_సూచీ = (vptr >> పుట_బిట్లు) & ముసుగు(PT_సూచీ_బిట్లు);
    తిరిగి pt + pt_సూచీ;
}

స్థిర స్థిరరాశి resolve_ret_t default_resolve_ret_t;

స్థిర resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    ఎంపిక (pde_ptr_get_pdeType(pde)) {
    స్థితి pde_pde_section:
        ret.చట్రం_పునాది = pde_pde_section_ptr_get_చిరునామా(pde);
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (pde_pde_section_ptr_get_కొలత(pde)) {
            ret.చట్రం_కొలత = ARMSuperSection;
        } లేకపోతే {
            ret.చట్రం_కొలత = ARMSection;
        }
#లేకపోతే
        ఒకవేళ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.చట్రం_పునాది &= ~ముసుగు(పుట_బిట్లు_For_కొలత(ARMSuperSection));
            ret.చట్రం_కొలత = ARMSuperSection;
        } లేకపోతే {
            ret.చట్రం_కొలత = ARMSection;
        }
#అయితే_ముగింపు
        తిరిగి ret;

    స్థితి pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_చిరునామా(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        ఎంపిక (pte_ptr_get_pteType(pte)) {
        స్థితి pte_pte_small:
            ret.చట్రం_పునాది = pte_pte_small_ptr_get_చిరునామా(pte);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            ఒకవేళ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.చట్రం_పునాది &= ~ముసుగు(పుట_బిట్లు_For_కొలత(ARMLarge_పుట));
                ret.చట్రం_కొలత = ARMLarge_పుట;
            } లేకపోతే {
                ret.చట్రం_కొలత = ARMSmall_పుట;
            }
#లేకపోతే
            ret.చట్రం_కొలత = ARMSmall_పుట;
#అయితే_ముగింపు
            తిరిగి ret;

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        స్థితి pte_pte_large:
            ret.చట్రం_పునాది = pte_pte_large_ptr_get_చిరునామా(pte);
            ret.చట్రం_కొలత = ARMLarge_పుట;
            తిరిగి ret;
#అయితే_ముగింపు
        అప్రమేయ:
            ఆపు;
        }
        ఆపు;
    }
    }

    ret.valid = false;
    తిరిగి ret;
}

స్థిర pte_t CONST makeUserPTE(vm_పుట_కొలత_t పుట_కొలత, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t ap;

    ap = APFromVMRights(vm_rights);

    ఎంపిక (పుట_కొలత) {
    స్థితి ARMSmall_పుట: {
        ఒకవేళ (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } లేకపోతే {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        ఆపు;
    }

    స్థితి ARMLarge_పుట: {
        ఒకవేళ (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } లేకపోతే {
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
        ఆపు;
    }

    అప్రమేయ:
        fail("Invalid PTE frame type");
    }

#లేకపోతే

    మాట_t hap;

    hap = HAPFromVMRights(vm_rights);

    ఎంపిక (పుట_కొలత) {
    స్థితి ARMSmall_పుట: {
        ఒకవేళ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } లేకపోతే {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ఆపు;
    }

    స్థితి ARMLarge_పుట: {
        ఒకవేళ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } లేకపోతే {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ఆపు;
    }
    అప్రమేయ:
        fail("Invalid PTE frame type");
    }
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    తిరిగి pte;
}

స్థిర pde_t CONST makeUserPDE(vm_పుట_కొలత_t పుట_కొలత, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, మాట_t domain,
                               vm_rights_t vm_rights)
{
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t ap, size2;

    ap = APFromVMRights(vm_rights);
#లేకపోతే
    మాట_t hap, size2;

    (శూన్య)domain;
    hap = HAPFromVMRights(vm_rights);
#అయితే_ముగింపు

    ఎంపిక (పుట_కొలత) {
    స్థితి ARMSection:
        size2 = 0;
        ఆపు;

    స్థితి ARMSuperSection:
        size2 = 1;
        ఆపు;

    అప్రమేయ:
        fail("Invalid PDE frame type");
    }

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    ఒకవేళ (cacheable) {
        తిరిగి pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } లేకపోతే {
        తిరిగి pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#లేకపోతే
    ఒకవేళ (cacheable) {
        తిరిగి pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } లేకపోతే {
        తిరిగి pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_మూలం(cap_t cap)
{
    తిరిగి cap_get_capType(cap) == cap_పుట_directory_cap &&
           cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_చట్రం_Cap(cap_t cap)
{
#నిర్వచితమైతే CONFIG_TK1_SMMU
    తిరిగి cap_get_capType(cap) == cap_small_చట్రం_cap && cap_small_చట్రం_cap_get_cap_FIs_IOSpace(cap);
#లేకపోతే
    తిరిగి false;
#అయితే_ముగింపు
}

/** Switch to the empty PD on the reserved HW ASID */
స్థిర శూన్య సమితి_Global_PD(శూన్య)
{
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#లేకపోతే
    /* Before changing the PD ensure all memory accesses have completed */
    dsb();
    /* First switch to global PD on old ASID. Stale TLB entries may exist
       under the old ASID, but no new stale mappings can be added any more. */
    writeTTBR0Ptr(addrFromKPPtr(armKSGlobalPD));
    /* Ensure the PD switch completes before we do anything else */
    isb();
    /* Switch to reserved HW ASID. Only empty/global kernel mappings are
       now available from the TLB. */
    సమితి_Hardware_ASID(hwASIDReserved);
#అయితే_ముగింపు
}

శూన్య సమితి_VMRoot(tcb_t *tcb)
{
    cap_t thread_మూలం;
    asid_t asid;
    pde_t *pd;
    కనుగొను_PDFor_ASID_ret_t కనుగొను_ret;

    thread_మూలం = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ఒకవేళ (cap_get_capType(thread_మూలం) != cap_పుట_directory_cap ||
        !cap_పుట_directory_cap_get_cap_PDIs_Mapped(thread_మూలం)) {
        సమితి_Global_PD();
        తిరిగి;
    }

    pd = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(thread_మూలం));
    asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(thread_మూలం);
    కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE || కనుగొను_ret.pd != pd)) {
        సమితి_Global_PD();
        తిరిగి;
    }

    armv_contextSwitch(pd, asid);
}

స్థిర bool_t సమితి_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_మూలం;

    thread_మూలం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbVTable)->cap;

    ఒకవేళ (cap_get_capType(thread_మూలం) == cap_పుట_directory_cap &&
        cap_పుట_directory_cap_get_cap_PDIs_Mapped(thread_మూలం) &&
        PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(thread_మూలం)) == pd) {
        తిరిగి false;
    }

    armv_contextSwitch(pd, asid);

    తిరిగి true;
}

pde_t *పుట_పట్టిక_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    కనుగొను_PDFor_ASID_ret_t కనుగొను_ret;
    pde_t pde;
    గుర్తులేని పూర్ణాంకం pd_సూచీ;

    కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
        తిరిగి ఖాళీ;
    }

    pd_సూచీ = vaddr >> (పుట_బిట్లు + PT_సూచీ_బిట్లు);
    pde = కనుగొను_ret.pd[pd_సూచీ];

    ఒకవేళ (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_చిరునామా(pde)) == pt)) {
        తిరిగి కనుగొను_ret.pd;
    } లేకపోతే {
        తిరిగి ఖాళీ;
    }
}

స్థిర శూన్య invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ఎగువ(asid)];
    ప్రతిజ్ఞా(asidPool);

    pd = asidPool->array[ASID_దిగువ(asid)];
    ప్రతిజ్ఞా(pd);

    pd[PD_ASID_స్థానం] = pde_pde_invalid_new(0, false);
}

స్థిర pde_t శుద్ధ ఎక్కించు_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ఎగువ(asid)];
    ప్రతిజ్ఞా(asidPool);

    pd = asidPool->array[ASID_దిగువ(asid)];
    ప్రతిజ్ఞా(pd);

    తిరిగి pd[PD_ASID_స్థానం];
}

స్థిర శూన్య storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ఎగువ(asid)];
    ప్రతిజ్ఞా(asidPool);

    pd = asidPool->array[ASID_దిగువ(asid)];
    ప్రతిజ్ఞా(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_స్థానం] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t కనుగొను_Free_HWASID(శూన్య)
{
    మాట_t hw_asid_స్థానభ్రంశం;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    కొరకు (hw_asid_స్థానభ్రంశం = 0;
         hw_asid_స్థానభ్రంశం <= (మాట_t)((hw_asid_t) - 1);
         hw_asid_స్థానభ్రంశం ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_స్థానభ్రంశం);
        ఒకవేళ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            తిరిగి hw_asid;
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
    ఒకవేళ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    తిరిగి hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ఎక్కించు_HWASID(asid);
    ఒకవేళ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        తిరిగి pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } లేకపోతే {
        hw_asid_t new_hw_asid;

        new_hw_asid = కనుగొను_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        తిరిగి new_hw_asid;
    }
}

స్థిర శూన్య invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ఎక్కించు_HWASID(asid);
    ఒకవేళ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

శూన్య unmap_పుట_పట్టిక(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_స్థానం;
    గుర్తులేని పూర్ణాంకం pd_సూచీ;

    pd = పుట_పట్టిక_Mapped(asid, vaddr, pt);

    ఒకవేళ (likely(pd != ఖాళీ)) {
        pd_సూచీ = vaddr >> (PT_సూచీ_బిట్లు + పుట_బిట్లు);
        pd_స్థానం = pd + pd_సూచీ;

        *pd_స్థానం = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((మాట_t)pd_స్థానం, addrFromPPtr(pd_స్థానం));
        flush_పట్టిక(pd, asid, vaddr, pt);
    }
}

శూన్య copyGlobalMappings(pde_t *newPD)
{
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t i;
    pde_t *global_pd = armKSGlobalPD;

    కొరకు (i = PPTR_పునాది >> ARMSection_బిట్లు; i < బిట్టు(PD_సూచీ_బిట్లు); i++) {
        ఒకవేళ (i != PD_ASID_స్థానం) {
            newPD[i] = global_pd[i];
        }
    }
#అయితే_ముగింపు
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ఎంపిక (vm_faultType) {
    స్థితి ARMDataAbort: {
        మాట_t addr, fault;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (చిరునామా_Translate_S1(addr) & ~ముసుగు(పుట_బిట్లు)) | (addr & ముసుగు(పుట_బిట్లు));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#లేకపోతే
        addr = getFAR();
        fault = getDFSR();
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        ఒకవేళ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            తిరిగి EXCEPTION_FAULT;
        }
#అయితే_ముగింపు
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        తిరిగి EXCEPTION_FAULT;
    }

    స్థితి ARMPrefetchAbort: {
        మాట_t pc, fault;

        pc = getRestartPC(thread);

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (చిరునామా_Translate_S1(pc) & ~ముసుగు(పుట_బిట్లు)) | (pc & ముసుగు(పుట_బిట్లు));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#లేకపోతే
        fault = getIFSR();
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
        ఒకవేళ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            ఒకవేళ (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_అడుగు
                && !single_అడుగు_Fault_Counter_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                తిరిగి EXCEPTION_NONE;
            }
            తిరిగి EXCEPTION_FAULT;
        }
#అయితే_ముగింపు
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        తిరిగి EXCEPTION_FAULT;
    }

    అప్రమేయ:
        fail("Invalid VM fault type");
    }
}

శూన్య deleteASIDPool(asid_t asid_పునాది, asid_pool_t *pool)
{
    గుర్తులేని పూర్ణాంకం స్థానభ్రంశం;

    /* Haskell error: "ASID pool's base must be aligned" */
    ప్రతిజ్ఞా((asid_పునాది & ముసుగు(asid_దిగువ_బిట్లు)) == 0);

    ఒకవేళ (armKSASIDTable[ASID_ఎగువ(asid_పునాది)] == pool) {
        కొరకు (స్థానభ్రంశం = 0; స్థానభ్రంశం < బిట్టు(asid_దిగువ_బిట్లు); స్థానభ్రంశం++) {
            ఒకవేళ (pool->array[స్థానభ్రంశం]) {
                flushSpace(asid_పునాది + స్థానభ్రంశం);
                invalidateASIDEntry(asid_పునాది + స్థానభ్రంశం);
            }
        }
        armKSASIDTable[ASID_ఎగువ(asid_పునాది)] = ఖాళీ;
        సమితి_VMRoot(కణుపు_STATE(ksCurThread));
    }
}

శూన్య deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ఎగువ(asid)];

    ఒకవేళ (poolPtr != ఖాళీ && poolPtr->array[ASID_దిగువ(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_దిగువ(asid)] = ఖాళీ;
        సమితి_VMRoot(కణుపు_STATE(ksCurThread));
    }
}

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
స్థిర pte_t pte_pte_invalid_new(శూన్య)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    తిరిగి (pte_t) {
        {
            0
        }
    };
}
#అయితే_ముగింపు

శూన్య unmap_పుట(vm_పుట_కొలత_t పుట_కొలత, asid_t asid, vptr_t vptr, శూన్య *pptr)
{
    కనుగొను_PDFor_ASID_ret_t కనుగొను_ret;
    paddr_t addr = addrFromPPtr(pptr);

    కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
        తిరిగి;
    }

    ఎంపిక (పుట_కొలత) {
    స్థితి ARMSmall_పుట: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(కనుగొను_ret.pd, vptr);
        ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
            తిరిగి;
        }

        ఒకవేళ (unlikely(pte_ptr_get_pteType(lu_ret.pt_స్థానం) != pte_pte_small)) {
            తిరిగి;
        }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_స్థానం) != 0)) {
            తిరిగి;
        }
#అయితే_ముగింపు
        ఒకవేళ (unlikely(pte_pte_small_ptr_get_చిరునామా(lu_ret.pt_స్థానం) != addr)) {
            తిరిగి;
        }

        *(lu_ret.pt_స్థానం) = pte_pte_invalid_new();
        cleanByVA_PoU((మాట_t)lu_ret.pt_స్థానం, addrFromPPtr(lu_ret.pt_స్థానం));

        ఆపు;
    }

    స్థితి ARMLarge_పుట: {
        lookupPTSlot_ret_t lu_ret;
        మాట_t i;

        lu_ret = lookupPTSlot(కనుగొను_ret.pd, vptr);
        ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
            తిరిగి;
        }
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (unlikely(pte_ptr_get_pteType(lu_ret.pt_స్థానం) != pte_pte_large)) {
            తిరిగి;
        }
        ఒకవేళ (unlikely(pte_pte_large_ptr_get_చిరునామా(lu_ret.pt_స్థానం) != addr)) {
            తిరిగి;
        }
#లేకపోతే
        ఒకవేళ (unlikely(pte_ptr_get_pteType(lu_ret.pt_స్థానం) != pte_pte_small)) {
            తిరిగి;
        }
        ఒకవేళ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_స్థానం) != 1)) {
            తిరిగి;
        }
        ఒకవేళ (unlikely(pte_pte_small_ptr_get_చిరునామా(lu_ret.pt_స్థానం) != addr)) {
            తిరిగి;
        }
#అయితే_ముగింపు

        కొరకు (i = 0; i < పుటలు_చొప్పున_LARGE_పుట; i++) {
            lu_ret.pt_స్థానం[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((మాట_t)&lu_ret.pt_స్థానం[0],
                            చివరి_BYTE_PTE(lu_ret.pt_స్థానం, పుటలు_చొప్పున_LARGE_పుట),
                            addrFromPPtr(&lu_ret.pt_స్థానం[0]));

        ఆపు;
    }

    స్థితి ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(కనుగొను_ret.pd, vptr);

        ఒకవేళ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            తిరిగి;
        }
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (unlikely(pde_pde_section_ptr_get_కొలత(pd) != 0)) {
#లేకపోతే
        ఒకవేళ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#అయితే_ముగింపు
            తిరిగి;
        }
        ఒకవేళ (unlikely(pde_pde_section_ptr_get_చిరునామా(pd) != addr)) {
            తిరిగి;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((మాట_t)pd, addrFromPPtr(pd));

        ఆపు;
    }

    స్థితి ARMSuperSection: {
        pde_t *pd;
        మాట_t i;

        pd = lookupPDSlot(కనుగొను_ret.pd, vptr);

        ఒకవేళ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            తిరిగి;
        }
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (unlikely(pde_pde_section_ptr_get_కొలత(pd) != 1)) {
#లేకపోతే
        ఒకవేళ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#అయితే_ముగింపు
            తిరిగి;
        }
        ఒకవేళ (unlikely(pde_pde_section_ptr_get_చిరునామా(pd) != addr)) {
            తిరిగి;
        }

        కొరకు (i = 0; i < SECTIONS_చొప్పున_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((మాట_t)&pd[0], చివరి_BYTE_PDE(pd, SECTIONS_చొప్పున_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        ఆపు;
    }

    అప్రమేయ:
        fail("Invalid ARM page type");
        ఆపు;
    }

    /* Flush the page now that the mapping has been updated */
    flush_పుట(పుట_కొలత, కనుగొను_ret.pd, asid, vptr);
}

శూన్య flush_పుట(vm_పుట_కొలత_t పుట_కొలత, pde_t *pd, asid_t asid, మాట_t vptr)
{
    pde_t stored_hw_asid;
    మాట_t పునాది_addr;
    bool_t మూలం_switched;

    ప్రతిజ్ఞా((vptr & ముసుగు(పుట_బిట్లు_For_కొలత(పుట_కొలత))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    మూలం_switched = సమితి_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ఎక్కించు_HWASID(asid);

    ఒకవేళ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        పునాది_addr = vptr & ~ముసుగు(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(పునాది_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        ఒకవేళ (మూలం_switched) {
            సమితి_VMRoot(కణుపు_STATE(ksCurThread));
        }
    }
}

శూన్య flush_పట్టిక(pde_t *pd, asid_t asid, మాట_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t మూలం_switched;

    ప్రతిజ్ఞా((vptr & ముసుగు(PT_సూచీ_బిట్లు + ARMSmall_పుట_బిట్లు)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    మూలం_switched = సమితి_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ఎక్కించు_HWASID(asid);

    ఒకవేళ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        ఒకవేళ (మూలం_switched) {
            సమితి_VMRoot(కణుపు_STATE(ksCurThread));
        }
    }
}

శూన్య flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ఎక్కించు_HWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ఒకవేళ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        తిరిగి;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

శూన్య invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ఎక్కించు_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ఒకవేళ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        తిరిగి;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

స్థిర అంతఃవరుస bool_t CONST checkVPAlignment(vm_పుట_కొలత_t sz, మాట_t w)
{
    తిరిగి (w & ముసుగు(పుట_బిట్లు_For_కొలత(sz))) == 0;
}

నిర్మాణం create_mappings_pte_return {
    exception_t స్థితి_2;
    pte_t pte;
    pte_range_t pte_entries;
};
రకనిర్వచనం నిర్మాణం create_mappings_pte_return create_mappings_pte_return_t;

నిర్మాణం create_mappings_pde_return {
    exception_t స్థితి_2;
    pde_t pde;
    pde_range_t pde_entries;
};
రకనిర్వచనం నిర్మాణం create_mappings_pde_return create_mappings_pde_return_t;

స్థిర create_mappings_pte_return_t
create_సురక్షిత_Mapping_Entries_PTE
(paddr_t పునాది, మాట_t vaddr, vm_పుట_కొలత_t చట్రం_కొలత,
 vm_rights_t vmRights, vm_లక్షణాలు_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    మాట_t i;

    ఎంపిక (చట్రం_కొలత) {

    స్థితి ARMSmall_పుట:

        ret.pte_entries.పునాది = ఖాళీ; /* to avoid uninitialised warning */
        ret.pte_entries.నిడివి = 1;

        ret.pte = makeUserPTE(ARMSmall_పుట, పునాది,
                              vm_లక్షణాలు_get_arm_పుట_Cacheable(attr),
                              vm_లక్షణాలు_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం =
                false;
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            తిరిగి ret;
        }

        ret.pte_entries.పునాది = lu_ret.pt_స్థానం;
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (unlikely(pte_ptr_get_pteType(ret.pte_entries.పునాది) ==
                     pte_pte_large)) {
#లేకపోతే
        ఒకవేళ (unlikely(pte_ptr_get_pteType(ret.pte_entries.పునాది) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.పునాది))) {
#అయితే_ముగింపు
            current_syscall_error.type =
                se_L4_Delete_మొదటి;

            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
            తిరిగి ret;
        }

        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    స్థితి ARMLarge_పుట:

        ret.pte_entries.పునాది = ఖాళీ; /* to avoid uninitialised warning */
        ret.pte_entries.నిడివి = పుటలు_చొప్పున_LARGE_పుట;

        ret.pte = makeUserPTE(ARMLarge_పుట, పునాది,
                              vm_లక్షణాలు_get_arm_పుట_Cacheable(attr),
                              vm_లక్షణాలు_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం =
                false;
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            తిరిగి ret;
        }

        ret.pte_entries.పునాది = lu_ret.pt_స్థానం;

        కొరకు (i = 0; i < పుటలు_చొప్పున_LARGE_పుట; i++) {
            ఒకవేళ (unlikely(pte_get_pteType(ret.pte_entries.పునాది[i]) ==
                         pte_pte_small)
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.పునాది[i])
#అయితే_ముగింపు
               ) {
                current_syscall_error.type =
                    se_L4_Delete_మొదటి;

                ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
                తిరిగి ret;
            }
        }

        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    అప్రమేయ:
        fail("Invalid or unexpected ARM page type.");

    }
}

స్థిర create_mappings_pde_return_t
create_సురక్షిత_Mapping_Entries_PDE
(paddr_t పునాది, మాట_t vaddr, vm_పుట_కొలత_t చట్రం_కొలత,
 vm_rights_t vmRights, vm_లక్షణాలు_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_ట్యాగ్_t currentPDEType;
    మాట_t i;

    ఎంపిక (చట్రం_కొలత) {

    /* PDE mappings */
    స్థితి ARMSection:
        ret.pde_entries.పునాది = lookupPDSlot(pd, vaddr);
        ret.pde_entries.నిడివి = 1;

        ret.pde = makeUserPDE(ARMSection, పునాది,
                              vm_లక్షణాలు_get_arm_Parity_Enabled(attr),
                              vm_లక్షణాలు_get_arm_పుట_Cacheable(attr),
                              vm_లక్షణాలు_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.పునాది);
        ఒకవేళ (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_కొలత(ret.pde_entries.పునాది) != 0))) {
#లేకపోతే
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.పునాది) != 0))) {
#అయితే_ముగింపు
            current_syscall_error.type =
                se_L4_Delete_మొదటి;
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;

            తిరిగి ret;
        }

        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    స్థితి ARMSuperSection:
        ret.pde_entries.పునాది = lookupPDSlot(pd, vaddr);
        ret.pde_entries.నిడివి = SECTIONS_చొప్పున_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, పునాది,
                              vm_లక్షణాలు_get_arm_Parity_Enabled(attr),
                              vm_లక్షణాలు_get_arm_పుట_Cacheable(attr),
                              vm_లక్షణాలు_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        కొరకు (i = 0; i < SECTIONS_చొప్పున_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.పునాది[i]);
            ఒకవేళ (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_కొలత(ret.pde_entries.పునాది[i]) != 1))) {
#లేకపోతే
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.పునాది[i]) != 1))) {
#అయితే_ముగింపు
                current_syscall_error.type =
                    se_L4_Delete_మొదటి;
                ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;

                తిరిగి ret;
            }
        }

        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    అప్రమేయ:
        fail("Invalid or unexpected ARM page type.");

    }
}

స్థిర అంతఃవరుస vptr_t పుట_పునాది(vptr_t vaddr, vm_పుట_కొలత_t కొలత)
{
    తిరిగి vaddr & ~ముసుగు(పుట_బిట్లు_For_కొలత(కొలత));
}

స్థిర bool_t శుద్ధ pteCheckIfMapped(pte_t *pte)
{
    తిరిగి pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

స్థిర bool_t శుద్ధ pdeCheckIfMapped(pde_t *pde)
{
    తిరిగి pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

స్థిర శూన్య doFlush(పూర్ణాంకం invLabel, vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        చివర = (vptr_t)paddr_గమ్యం_pptr(pstart) + (చివర - ప్రారంభం);
        ప్రారంభం = (vptr_t)paddr_గమ్యం_pptr(pstart);
    }
    ఎంపిక (invLabel) {
    స్థితి ARMPDClean_దత్తాంశం:
    స్థితి ARMPage_Clean_దత్తాంశం:
        cleanCacheRange_RAM(ప్రారంభం, చివర, pstart);
        ఆపు;
    స్థితి ARMPDInvalidate_దత్తాంశం:
    స్థితి ARMPage_Invalidate_దత్తాంశం:
        invalidateCacheRange_RAM(ప్రారంభం, చివర, pstart);
        ఆపు;
    స్థితి ARMPDClean_Invalidate_దత్తాంశం:
    స్థితి ARMPage_Clean_Invalidate_దత్తాంశం:
        cleanInvalidateCacheRange_RAM(ప్రారంభం, చివర, pstart);
        ఆపు;
    స్థితి ARMPDUnify_Instruction:
    స్థితి ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(ప్రారంభం, చివర, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(ప్రారంభం, చివర, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(ప్రారంభం, చివర, pstart);
        /* ... then wait for the completion of invalidations. */
        dsb();
        /* Ensure new instructions come from fresh cache lines. */
        isb();
        ఆపు;
    అప్రమేయ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

స్థిర exception_t performPDFlush(పూర్ణాంకం invLabel, pde_t *pd, asid_t asid, vptr_t ప్రారంభం,
                                  vptr_t చివర, paddr_t pstart)
{
    bool_t మూలం_switched;

    /* Flush if given a non zero range */
    ఒకవేళ (ప్రారంభం < చివర) {
        మూలం_switched = సమితి_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, ప్రారంభం, చివర, pstart);

        ఒకవేళ (మూలం_switched) {
            సమితి_VMRoot(కణుపు_STATE(ksCurThread));
        }
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_పట్టిక_Invocation_Map(cap_t cap, cte_t *ct_స్థానం,
                                                 pde_t pde, pde_t *pd_స్థానం)
{
    ct_స్థానం->cap = cap;
    *pd_స్థానం = pde;
    cleanByVA_PoU((మాట_t)pd_స్థానం, addrFromPPtr(pd_స్థానం));

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_పట్టిక_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
        unmap_పుట_పట్టిక(
            cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
            cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap),
            pt);
        clear_జ్ఞాపకం_PT((శూన్య *)pt, cap_get_cap_కొలత_బిట్లు(cap));
    }
    cap_పుట_పట్టిక_cap_ptr_సమితి_cap_PTIs_Mapped(&(ct_స్థానం->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_స్థానం, pte_t pte,
                                               pte_range_t pte_entries)
{
    మాట_t i, j వాడని;
    bool_t tlbflush_required;

    ct_స్థానం->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.పునాది);

    j = pte_entries.నిడివి;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t పునాది_చిరునామా = pte_pte_small_get_చిరునామా(pte);
#అయితే_ముగింపు
    కొరకు (i = 0; i < pte_entries.నిడివి; i++) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_సమితి_చిరునామా(pte, పునాది_చిరునామా + i * బిట్టు(పుట_బిట్లు_For_కొలత(ARMSmall_పుట)));
#అయితే_ముగింపు
        pte_entries.పునాది[i] = pte;
    }
    cleanCacheRange_PoU((మాట_t)pte_entries.పునాది,
                        చివరి_BYTE_PTE(pte_entries.పునాది, pte_entries.నిడివి),
                        addrFromPPtr(pte_entries.పునాది));
    ఒకవేళ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_స్థానం, pde_t pde,
                                               pde_range_t pde_entries)
{
    మాట_t i, j వాడని;
    bool_t tlbflush_required;

    ct_స్థానం->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.పునాది);

    j = pde_entries.నిడివి;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t పునాది_చిరునామా = pde_pde_section_get_చిరునామా(pde);
#అయితే_ముగింపు
    కొరకు (i = 0; i < pde_entries.నిడివి; i++) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_సమితి_చిరునామా(pde, పునాది_చిరునామా + i * బిట్టు(పుట_బిట్లు_For_కొలత(ARMSection)));
#అయితే_ముగింపు
        pde_entries.పునాది[i] = pde;
    }
    cleanCacheRange_PoU((మాట_t)pde_entries.పునాది,
                        చివరి_BYTE_PDE(pde_entries.పునాది, pde_entries.నిడివి),
                        addrFromPPtr(pde_entries.పునాది));
    ఒకవేళ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (generic_చట్రం_cap_get_cap_FIs_Mapped(cap)) {
        unmap_పుట(generic_చట్రం_cap_get_cap_FSize(cap),
                  generic_చట్రం_cap_get_cap_FMapped_ASID(cap),
                  generic_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                  (శూన్య *)generic_చట్రం_cap_get_cap_FBase_Ptr(cap));
    }

    generic_చట్రం_cap_ptr_సమితి_cap_FMapped_చిరునామా(&ct_స్థానం->cap, asidInvalid, 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Flush(పూర్ణాంకం invLabel, pde_t *pd, asid_t asid, vptr_t ప్రారంభం,
                                    vptr_t చివర, paddr_t pstart)
{
    bool_t మూలం_switched;

    /* now we can flush. But only if we were given a non zero range */
    ఒకవేళ (ప్రారంభం < చివర) {
        మూలం_switched = సమితి_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, ప్రారంభం, చివర, pstart);

        ఒకవేళ (మూలం_switched) {
            సమితి_VMRoot(కణుపు_STATE(ksCurThread));
        }
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Get_చిరునామా(శూన్య *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, ipc_బఫరు, 0, capFBasePtr);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_స్థానం)
{
    cap_పుట_directory_cap_ptr_సమితి_cap_PDMapped_ASID(&pd_Cap_స్థానం->cap, asid);
    cap_పుట_directory_cap_ptr_సమితి_cap_PDIs_Mapped(&pd_Cap_స్థానం->cap, 1);
    poolPtr->array[ASID_దిగువ(asid)] =
        PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(pd_Cap_స్థానం->cap));

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performASIDControlInvocation(శూన్య *చట్రం, cte_t *స్థానం,
                                                cte_t *మాతృ, asid_t asid_పునాది)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_సమితి_cap_Free_సూచీ(&(మాతృ->cap),
                                         MAX_FREE_సూచీ(cap_untyped_cap_get_cap_ఖండం_కొలత(మాతృ->cap)));

    memzero(చట్రం, 1 << ARMSmall_పుట_బిట్లు);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_చొప్పించు(cap_asid_pool_cap_new(asid_పునాది, మాట_REF(చట్రం)),
              మాతృ, స్థానం);;
    /* Haskell error: "ASID pool's base must be aligned" */
    ప్రతిజ్ఞా((asid_పునాది & ముసుగు(asid_దిగువ_బిట్లు)) == 0);
    armKSASIDTable[ASID_ఎగువ(asid_పునాది)] = (asid_pool_t *)చట్రం;

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeARMPageDirectoryInvocation(మాట_t invLabel, మాట_t నిడివి,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    మాట_t *బఫరు)
{
    ఎంపిక (invLabel) {
    స్థితి ARMPDClean_దత్తాంశం:
    స్థితి ARMPDInvalidate_దత్తాంశం:
    స్థితి ARMPDClean_Invalidate_దత్తాంశం:
    స్థితి ARMPDUnify_Instruction: {
        vptr_t ప్రారంభం, చివర;
        paddr_t pstart;
        కనుగొను_PDFor_ASID_ret_t కనుగొను_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        ఒకవేళ (నిడివి < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ప్రారంభం = getSyscallArg(0, బఫరు);
        చివర =   getSyscallArg(1, బఫరు);

        /* Check sanity of arguments */
        ఒకవేళ (చివర <= ప్రారంభం) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ఒకవేళ (ప్రారంభం >= USER_శిఖరం || చివర > USER_శిఖరం) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(cap_get_capType(cap) != cap_పుట_directory_cap ||
                     !cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap);

        కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
        ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(కనుగొను_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, ప్రారంభం);

        /* Check that there's actually something there. */
        ఒకవేళ (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ఒకవేళ (పుట_పునాది(ప్రారంభం, resolve_ret.చట్రం_కొలత) !=
            పుట_పునాది(చివర - 1, resolve_ret.చట్రం_కొలత)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = ప్రారంభం;
            current_syscall_error.rangeErrorMax =
                పుట_పునాది(ప్రారంభం, resolve_ret.చట్రం_కొలత) +
                ముసుగు(పుట_బిట్లు_For_కొలత(resolve_ret.చట్రం_కొలత));
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.చట్రం_పునాది
                 + (ప్రారంభం & ముసుగు(పుట_బిట్లు_For_కొలత(resolve_ret.చట్రం_కొలత)));

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ఒకవేళ (pstart < phys_పునాది() || pstart > PADDR_శిఖరం || (చివర - ప్రారంభం) > PADDR_శిఖరం - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performPDFlush(invLabel, pd, asid, ప్రారంభం, చివర - 1, pstart);
    }

    అప్రమేయ:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

}

స్థిర exception_t decode_ARMPage_పట్టిక_Invocation(మాట_t invLabel, మాట_t నిడివి,
                                                cte_t *cte, cap_t cap, మాట_t *బఫరు)
{
    మాట_t vaddr, pd_సూచీ;

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_లక్షణాలు_t attr;
#అయితే_ముగింపు
    cap_t pdCap;
    pde_t *pd, *pd_స్థానం;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    ఒకవేళ (invLabel == ARMPage_పట్టిక_Unmap) {
        ఒకవేళ (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_పట్టిక_Invocation_Unmap(cap, cte);
    }

    ఒకవేళ (unlikely(invLabel != ARMPage_పట్టిక_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు);
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_లక్షణాలు_From_మాట(getSyscallArg(1, బఫరు));
#అయితే_ముగింపు
    pdCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (unlikely(cap_get_capType(pdCap) != cap_పుట_directory_cap ||
                 !cap_పుట_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(pdCap);

    ఒకవేళ (unlikely(vaddr >= USER_శిఖరం)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_శిఖరం);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    {
        కనుగొను_PDFor_ASID_ret_t కనుగొను_ret;

        కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
        ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(కనుగొను_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_సూచీ = vaddr >> (పుట_బిట్లు + PT_సూచీ_బిట్లు);
    pd_స్థానం = &pd[pd_సూచీ];
    ఒకవేళ (unlikely(pde_ptr_get_pdeType(pd_స్థానం) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_మొదటి;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap)));
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_లక్షణాలు_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#లేకపోతే
    pde = pde_pde_coarse_new(paddr);
#అయితే_ముగింపు

    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTIs_Mapped(cap, 1);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_ASID(cap, asid);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి perform_పుట_పట్టిక_Invocation_Map(cap, cte, pde, pd_స్థానం);
}

స్థిర exception_t decodeARMFrameInvocation(మాట_t invLabel, మాట_t నిడివి,
                                            cte_t *cte, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఎంపిక (invLabel) {
    స్థితి ARMPageMap: {
        మాట_t vaddr, vtop, w_rights_ముసుగు;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_పుట_కొలత_t చట్రం_కొలత;
        vm_లక్షణాలు_t attr;

        ఒకవేళ (unlikely(నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, బఫరు);
        w_rights_ముసుగు = getSyscallArg(1, బఫరు);
        attr = vm_లక్షణాలు_From_మాట(getSyscallArg(2, బఫరు));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        చట్రం_కొలత = generic_చట్రం_cap_get_cap_FSize(cap);
        capVMRights = generic_చట్రం_cap_get_cap_FVMRights(cap);

        ఒకవేళ (unlikely(cap_get_capType(pdCap) != cap_పుట_directory_cap ||
                     !cap_పుట_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(pdCap);

        ఒకవేళ (generic_చట్రం_cap_get_cap_FIs_Mapped(cap)) {
            ఒకవేళ (generic_చట్రం_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_సంఖ్య = 1;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            ఒకవేళ (generic_చట్రం_cap_get_cap_FMapped_చిరునామా(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        } లేకపోతే {
            vtop = vaddr + బిట్టు(పుట_బిట్లు_For_కొలత(చట్రం_కొలత)) - 1;

            ఒకవేళ (unlikely(vtop >= USER_శిఖరం)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_శిఖరం);
                current_syscall_error.type =
                    se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            కనుగొను_PDFor_ASID_ret_t కనుగొను_ret;

            కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
            ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ఆధారం =
                    false;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            ఒకవేళ (unlikely(కనుగొను_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_సంఖ్య = 1;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            ముసుగు_VMRights(capVMRights, rights_From_మాట(w_rights_ముసుగు));

        ఒకవేళ (unlikely(!checkVPAlignment(చట్రం_కొలత, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((శూన్య *)
                                   generic_చట్రం_cap_get_cap_FBase_Ptr(cap));

        cap = generic_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, asid,
                                                      vaddr);
        ఒకవేళ (చట్రం_కొలత == ARMSmall_పుట || చట్రం_కొలత == ARMLarge_పుట) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_సురక్షిత_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   చట్రం_కొలత, vmRights,
                                                   attr, pd);
            ఒకవేళ (unlikely(map_ret.స్థితి_2 != EXCEPTION_NONE)) {
#నిర్వచితమైతే CONFIG_PRINTING
                ఒకవేళ (current_syscall_error.type == se_L4_Delete_మొదటి) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#అయితే_ముగింపు
                తిరిగి map_ret.స్థితి_2;
            }

            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి perform_పుట_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } లేకపోతే {
            create_mappings_pde_return_t map_ret;
            map_ret = create_సురక్షిత_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   చట్రం_కొలత, vmRights,
                                                   attr, pd);
            ఒకవేళ (unlikely(map_ret.స్థితి_2 != EXCEPTION_NONE)) {
#నిర్వచితమైతే CONFIG_PRINTING
                ఒకవేళ (current_syscall_error.type == se_L4_Delete_మొదటి) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#అయితే_ముగింపు
                తిరిగి map_ret.స్థితి_2;
            }

            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి perform_పుట_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    స్థితి ARMPageUnmap: {
#నిర్వచితమైతే CONFIG_TK1_SMMU
        ఒకవేళ (is_IOSpace_చట్రం_Cap(cap)) {
            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి perform_పుట_Invocation_Unmap_IO(cap, cte);
        } లేకపోతే
#అయితే_ముగింపు
        {
            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి perform_పుట_Invocation_Unmap(cap, cte);
        }
    }

#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి ARMPageMapIO: {
        తిరిగి decodeARMIOMapInvocation(invLabel, నిడివి, cte, cap, బఫరు);
    }
#అయితే_ముగింపు

    స్థితి ARMPage_Clean_దత్తాంశం:
    స్థితి ARMPage_Invalidate_దత్తాంశం:
    స్థితి ARMPage_Clean_Invalidate_దత్తాంశం:
    స్థితి ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        కనుగొను_PDFor_ASID_ret_t pd;
        vptr_t ప్రారంభం, చివర;
        paddr_t pstart;
        మాట_t పుట_కొలత;
        మాట_t పుట_పునాది_2;

        ఒకవేళ (నిడివి < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_చట్రం_cap_get_cap_FMapped_ASID(cap);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_చట్రం_cap_get_cap_FBase_Ptr(cap);
#లేకపోతే
        vaddr = generic_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
#అయితే_ముగింపు

        ఒకవేళ (unlikely(!generic_చట్రం_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pd = కనుగొను_PDFor_ASID(asid);
        ఒకవేళ (unlikely(pd.స్థితి_2 != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ప్రారంభం = getSyscallArg(0, బఫరు);
        చివర =   getSyscallArg(1, బఫరు);

        /* check that the range is sane */
        ఒకవేళ (చివర <= ప్రారంభం) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        పుట_కొలత = 1 << పుట_బిట్లు_For_కొలత(generic_చట్రం_cap_get_cap_FSize(cap));
        పుట_పునాది_2 = addrFromPPtr((శూన్య *)generic_చట్రం_cap_get_cap_FBase_Ptr(cap));

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        ఒకవేళ (unlikely(!resolve_ret.valid ||
                     resolve_ret.చట్రం_కొలత != generic_చట్రం_cap_get_cap_FSize(cap) ||
                     resolve_ret.చట్రం_పునాది != పుట_పునాది_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు

        ఒకవేళ (ప్రారంభం >= పుట_కొలత || చివర > పుట_కొలత) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = పుట_పునాది_2 + ప్రారంభం;
        ప్రారంభం += vaddr;
        చివర += vaddr;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ఒకవేళ (pstart < phys_పునాది() || pstart > PADDR_శిఖరం || (చివర - ప్రారంభం) > PADDR_శిఖరం - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Flush(invLabel, pd.pd, asid, ప్రారంభం, చివర - 1, pstart);
    }

    స్థితి ARMPage_Get_చిరునామా: {


        /* Check that there are enough message registers */
        ప్రతిజ్ఞా(n_సందేశం_Registers >= 1);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Get_చిరునామా((శూన్య *)generic_చట్రం_cap_get_cap_FBase_Ptr(cap), call);
    }

    అప్రమేయ:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(మాట_t invLabel, మాట_t నిడివి, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_directory_cap:
        తిరిగి decodeARMPageDirectoryInvocation(invLabel, నిడివి, cptr, cte,
                                                cap, బఫరు);

    స్థితి cap_పుట_పట్టిక_cap:
        తిరిగి decode_ARMPage_పట్టిక_Invocation(invLabel, నిడివి, cte, cap, బఫరు);

    స్థితి cap_small_చట్రం_cap:
    స్థితి cap_చట్రం_cap:
        తిరిగి decodeARMFrameInvocation(invLabel, నిడివి, cte, cap, call, బఫరు);

    స్థితి cap_asid_control_cap: {
        మాట_t i;
        asid_t asid_పునాది;
        మాట_t సూచీ, లోతు;
        cap_t untyped, మూలం;
        cte_t *మాతృ_స్థానం, *dest_స్థానం;
        lookup_స్థానం_ret_t lu_ret;
        శూన్య *చట్రం;
        exception_t స్థితి_2;

        ఒకవేళ (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ
                     || current_extra_caps.excaprefs[1] == ఖాళీ)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సూచీ = getSyscallArg(0, బఫరు);
        లోతు = getSyscallArg(1, బఫరు);
        మాతృ_స్థానం = current_extra_caps.excaprefs[0];
        untyped = మాతృ_స్థానం->cap;
        మూలం = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        కొరకు (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ఒకవేళ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid_పునాది = i << asid_దిగువ_బిట్లు;

        ఒకవేళ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_ఖండం_కొలత(untyped) !=
                     se_L4_ASIDPool_బిట్లు || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        స్థితి_2 = ensureNoChildren(మాతృ_స్థానం);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            తిరిగి స్థితి_2;
        }

        చట్రం = మాట_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_లక్ష్యం_స్థానం(మూలం, సూచీ, లోతు);
        ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            తిరిగి lu_ret.స్థితి_2;
        }
        dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDControlInvocation(చట్రం, dest_స్థానం,
                                            మాతృ_స్థానం, asid_పునాది);
    }

    స్థితి cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_స్థానం;
        asid_pool_t *pool;
        మాట_t i;
        asid_t asid;

        ఒకవేళ (unlikely(invLabel != ARMASIDPool_కేటాయించు)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(current_extra_caps.excaprefs[0] == ఖాళీ)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_స్థానం = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_స్థానం->cap;

        ఒకవేళ (unlikely(
                cap_get_capType(pdCap) != cap_పుట_directory_cap ||
                cap_పుట_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ఎగువ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ఒకవేళ (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            current_lookup_fault = lookup_fault_invalid_మూలం_new();

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        కొరకు (i = 0; i < (1 << asid_దిగువ_బిట్లు) && (asid + i == 0 || pool->array[i]); i++);

        ఒకవేళ (unlikely(i == 1 << asid_దిగువ_బిట్లు)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDPoolInvocation(asid, pool, pd_Cap_స్థానం);
    }

    అప్రమేయ:
        fail("Invalid ARM arch cap type");
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
exception_t benchmark_arch_map_log_బఫరు(మాట_t చట్రం_cptr)
{
    lookup_Cap_And_స్థానం_ret_t lu_ret;
    vm_పుట_కొలత_t చట్రం_కొలత;
    pptr_t  చట్రం_pptr;

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

    చట్రం_కొలత = generic_చట్రం_cap_get_cap_FSize(lu_ret.cap);

    ఒకవేళ (చట్రం_కొలత != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_pptr = generic_చట్రం_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_బఫరు = pptr_గమ్యం_paddr((శూన్య *) చట్రం_pptr);

    కొరకు (పూర్ణాంకం idx = 0; idx < బిట్టు(PT_సూచీ_బిట్లు); idx++) {
        paddr_t physical_చిరునామా = ks_User_Log_బఫరు + (idx << se_L4_పుట_బిట్లు);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_చిరునామా,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * బిట్టు(se_L4_పుట_బిట్లు)));
    }

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
శూన్య kernelPrefetchAbort(మాట_t pc, మాట_t lr) VISIBLE;
శూన్య kernel_దత్తాంశం_Abort(మాట_t pc) VISIBLE;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT

శూన్య kernelUndefinedInstruction(మాట_t pc) VISIBLE;

శూన్య kernelPrefetchAbort(మాట_t pc, మాట_t lr)
{
    రాయి("\n\nKERNEL PREFETCH ABORT!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("HSR: 0x%"SEL4_PRIx_మాట"\n", getHSR());
    రాయి("LR: 0x%"SEL4_PRIx_మాట"\n", lr);
    halt();
}

శూన్య kernel_దత్తాంశం_Abort(మాట_t pc)
{
    రాయి("\n\nKERNEL DATA ABORT!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("HDFAR: 0x%"SEL4_PRIx_మాట" HSR: 0x%"SEL4_PRIx_మాట"\n",
           getHDFAR(), getHSR());
    halt();
}

శూన్య kernelUndefinedInstruction(మాట_t pc)
{
    రాయి("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("HSR: 0x%"SEL4_PRIx_మాట"\n", getHSR());
    halt();
}

#లేకపోతే /* CONFIG_ARM_HYPERVISOR_SUPPORT */

శూన్య kernelPrefetchAbort(మాట_t pc, మాట_t lr)
{
    రాయి("\n\nKERNEL PREFETCH ABORT!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("IFSR: 0x%"SEL4_PRIx_మాట"\n", getIFSR());
    రాయి("LR: 0x%"SEL4_PRIx_మాట"\n", lr);
    halt();
}

శూన్య kernel_దత్తాంశం_Abort(మాట_t pc)
{
    రాయి("\n\nKERNEL DATA ABORT!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("FAR: 0x%"SEL4_PRIx_మాట" DFSR: 0x%"SEL4_PRIx_మాట"\n",
           getFAR(), getDFSR());
    halt();
}
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_PRINTING
రకనిర్వచనం నిర్మాణం read_మాట_From_VSpace_ret {
    exception_t స్థితి_2;
    మాట_t విలువ;
} read_మాట_From_VSpace_ret_t;

స్థిర read_మాట_From_VSpace_ret_t read_మాట_From_VSpace(pde_t *pd, మాట_t vaddr)
{
    read_మాట_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_స్థానం;
    pde_t *pd_స్థానం;
    paddr_t paddr;
    మాట_t స్థానభ్రంశం;
    pptr_t kernel_vaddr;
    మాట_t *విలువ;

    pd_స్థానం = lookupPDSlot(pd, vaddr);
    ఒకవేళ (pde_ptr_get_pdeType(pd_స్థానం) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_చిరునామా(pd_స్థానం);
        స్థానభ్రంశం = vaddr & ముసుగు(ARMSection_బిట్లు);
    } లేకపోతే {
        pt_స్థానం = lookupPTSlot(pd, vaddr);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (pt_స్థానం.స్థితి_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_స్థానం.pt_స్థానం) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_చిరునామా(pt_స్థానం.pt_స్థానం);
            ఒకవేళ (pte_pte_small_ptr_get_contiguous_hint(pt_స్థానం.pt_స్థానం)) {
                స్థానభ్రంశం = vaddr & ముసుగు(ARMLarge_పుట_బిట్లు);
            } లేకపోతే {
                స్థానభ్రంశం = vaddr & ముసుగు(ARMSmall_పుట_బిట్లు);
            }
#లేకపోతే
        ఒకవేళ (pt_స్థానం.స్థితి_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_స్థానం.pt_స్థానం) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_చిరునామా(pt_స్థానం.pt_స్థానం);
            స్థానభ్రంశం = vaddr & ముసుగు(ARMSmall_పుట_బిట్లు);
        } లేకపోతే ఒకవేళ (pt_స్థానం.స్థితి_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_స్థానం.pt_స్థానం) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_చిరునామా(pt_స్థానం.pt_స్థానం);
            స్థానభ్రంశం = vaddr & ముసుగు(ARMLarge_పుట_బిట్లు);
#అయితే_ముగింపు
        } లేకపోతే {
            ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
            తిరిగి ret;
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
    pde_t *pd;
    మాట_t sp;
    పూర్ణాంకం i;

    thread_మూలం = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ఒకవేళ (cap_get_capType(thread_మూలం) != cap_పుట_directory_cap) {
        రాయి("Invalid vspace\n");
        తిరిగి;
    }

    pd = (pde_t *)pptr_యొక్క_cap(thread_మూలం);

    sp = get_రిజిస్టరు(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ఒకవేళ (!IS_ALIGNED(sp, se_L4_మాట_కొలత_బిట్లు)) {
        రాయి("SP not aligned\n");
        తిరిగి;
    }

    కొరకు (i = 0; i < CONFIG_USER_దొంతర_TRACE_నిడివి; i++) {
        మాట_t చిరునామా = sp + (i * పరిమాణం(మాట_t));
        read_మాట_From_VSpace_ret_t ఫలితం;
        ఫలితం = read_మాట_From_VSpace(pd, చిరునామా);
        ఒకవేళ (ఫలితం.స్థితి_2 == EXCEPTION_NONE) {
            రాయి("0x%lx: 0x%lx\n", (దీర్ఘ)చిరునామా, (దీర్ఘ)ఫలితం.విలువ);
        } లేకపోతే {
            రాయి("0x%lx: INVALID\n", (దీర్ఘ)చిరునామా);
        }
    }
}
#అయితే_ముగింపు
