/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_IOMMU

#ضمن <kernel/boot.h>
#ضمن <machine.h>
#ضمن <machine/io.h>
#ضمن <arch/kernel/apic.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/acpi.h>
#ضمن <plat/machine/intel-vtd.h>
#ضمن <util.h>

#تعریف_کړه RTADDR_REG  0x20
#تعریف_کړه GCMD_REG    0x18
#تعریف_کړه GSTS_REG    0x1C
#تعریف_کړه CCMD_REG    0x28
#تعریف_کړه ECAP_REG    0x10
#تعریف_کړه IOTLB_REG   0x08
#تعریف_کړه FSTS_REG    0x34
#تعریف_کړه FECTL_REG   0x38
#تعریف_کړه FEDATA_REG  0x3C
#تعریف_کړه FEADDR_REG  0x40
#تعریف_کړه FEUADDR_REG 0x44
#تعریف_کړه CAP_REG     0x08

/* Bit Positions within Registers */
#تعریف_کړه SRTP        30  /* Set Root Table Pointer */
#تعریف_کړه RTPS        30  /* Root Table Pointer Status */
#تعریف_کړه TE          31  /* Translation Enable */
#تعریف_کړه TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#تعریف_کړه ICC         (31 + 32)  /* Invalidate Context Cache */
#تعریف_کړه CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#تعریف_کړه CAIG        27  /* Context Actual Invalidation Granularity */
#تعریف_کړه CAIG_ماسک   0x3
#تعریف_کړه IVO_ماسک    0x3FF
#تعریف_کړه IVT         31  /* Invalidate IOTLB */
#تعریف_کړه IIRG        28  /* IOTLB Invalidation Request Granularity */
#تعریف_کړه IAIG        25  /* IOTLB Actual Invalidation Granularity */
#تعریف_کړه IAIG_ماسک   0x7
#تعریف_کړه IP          30  /* Interrupt Pending */
#تعریف_کړه FRI         0x8 /* Fault Recording Index */
#تعریف_کړه FRI_ماسک    0xFF
#تعریف_کړه FRO         24
#تعریف_کړه FRO_ماسک    0xFF
#تعریف_کړه FI          12
#تعریف_کړه SID_ماسک    0xFFFF
#تعریف_کړه SID_BUS(a)  (ماسک(8) & (a >> 8))
#تعریف_کړه SID_DEV(a)  (ماسک(5) & (a >> 3))
#تعریف_کړه SID_FUNC(a) (ماسک(3) & a)
#تعریف_کړه FR_ماسک     0xFF
#تعریف_کړه FAULT_TYPE  30
#تعریف_کړه FAULT       31
#تعریف_کړه NFR         8   /* high word of CAP_REG */
#تعریف_کړه NFR_ماسک    0xff
#تعریف_کړه PPF         1
#تعریف_کړه PPF_ماسک    1
#تعریف_کړه PRESENT     1
#تعریف_کړه WBF         27
#تعریف_کړه WBFS        27
#تعریف_کړه DID         8
#تعریف_کړه RW          0x3

#تعریف_کړه SAGAW         8
#تعریف_کړه SAGAW_2_LEVEL 0x01
#تعریف_کړه SAGAW_3_LEVEL 0x02
#تعریف_کړه SAGAW_4_LEVEL 0x04
#تعریف_کړه SAGAW_5_LEVEL 0x08
#تعریف_کړه SAGAW_6_LEVEL 0x10

#تعریف_کړه CONTEXT_GLOBAL_INVALIDATE 0x1
#تعریف_کړه IOTLB_GLOBAL_INVALIDATE   0x1

#تعریف_کړه DMA_TLB_READ_DRAIN  بټ(17)
#تعریف_کړه DMA_TLB_WRITE_DRAIN بټ(16)

#تعریف_کړه N_VTD_CONTEXTS 256

تعريفنوع uint32_t drhu_id_t;

ساكن مضمن uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t دننه_واټن)
{
    أرجع *(متطاير uint32_t *)(PPTR_DRHU_پیل + (drhu_id << پاڼه_بټونه) + دننه_واټن);
}

ساكن مضمن فراغ vtd_write32(drhu_id_t drhu_id, uint32_t دننه_واټن, uint32_t ارزښت)
{
    *(متطاير uint32_t *)(PPTR_DRHU_پیل + (drhu_id << پاڼه_بټونه) + دننه_واټن) = ارزښت;
}


ساكن مضمن uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t دننه_واټن)
{
    أرجع *(متطاير uint64_t *)(PPTR_DRHU_پیل + (drhu_id << پاڼه_بټونه) + دننه_واټن);
}

ساكن مضمن فراغ vtd_write64(drhu_id_t drhu_id, uint32_t دننه_واټن, uint64_t ارزښت)
{
    *(متطاير uint64_t *)(PPTR_DRHU_پیل + (drhu_id << پاڼه_بټونه) + دننه_واټن) = ارزښت;
}

ساكن مضمن uint32_t get_ivo(drhu_id_t drhu_id)
{
    أرجع ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_ماسک) * 16;
}

ساكن uint32_t get_fro_دننه_واټن(drhu_id_t drhu_id)
{
    uint32_t fro_دننه_واټن;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_دننه_واټن = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_ماسک;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_دننه_واټن |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    أرجع fro_دننه_واټن << 4;
}

فراغ invalidate_context_cache(فراغ)
{
    /* FIXME - bugzilla bug 172
     * 1. Instead of assuming global invalidation, this function should
     *    accept a parameter to control the granularity of invalidation
     *    request.
     * 2. Instead of doing invalidation for all the IOMMUs, it should
     *    only do it for the IOMMU responsible for the requesting PCI
     *    device.
     */

    drhu_id_t i;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        طالما ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        طالما ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

فراغ invalidate_iotlb(فراغ)
{
    /* FIXME - bugzilla bug 172
     * 1. Instead of assuming global invalidation, this function should
     *    accept a parameter to control the granularity of invalidation
     *    request.
     * 2. Instead of doing invalidation for all the IOMMUs, it should
     *    only do it for the IOMMU responsible for the requesting PCI
     *    device.
     */

    uint8_t   invalidate_command = IOTLB_GLOBAL_INVALIDATE;
    uint32_t  iotlb_reg_پاسنی;
    uint32_t  ivo_دننه_واټن;
    drhu_id_t i;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        ivo_دننه_واټن = get_ivo(i);

        /* Wait till IVT bit is clear */
        طالما ((vtd_read32(i, ivo_دننه_واټن + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_پاسنی = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_پاسنی |= بټ(IVT);
        iotlb_reg_پاسنی |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_دننه_واټن + IOTLB_REG, 0);
        vtd_write32(i, ivo_دننه_واټن + IOTLB_REG + 4, iotlb_reg_پاسنی);

        /* Wait for the invalidation to complete */
        طالما ((vtd_read32(i, ivo_دننه_واټن + IOTLB_REG + 4) >> IVT) & 1);
    }
}

ساكن فراغ vtd_clear_fault(drhu_id_t i, کلمه_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, بټ(FAULT));
}

ساكن فراغ vtd_process_تېروتنې(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_دننه_واټن;
    uint32_t سرچینه_id ناکارول_شوی;
    uint32_t fault_type ناکارول_شوی;
    uint32_t پته[2] ناکارول_شوی;
    uint32_t reason ناکارول_شوی;
    uint32_t عدد_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_حالت;
    uint32_t fault_ریکارډ_شاخص;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_دننه_واټن = get_fro_دننه_واټن(i);
    fault_حالت = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_ماسک;

    إذا (fault_حالت) {
        عدد_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_ماسک) + 1;
        fault_ریکارډ_شاخص = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_ماسک;
        fr_reg = fro_دننه_واټن + 16 * fault_ریکارډ_شاخص;

        /* Traverse the fault register ring buffer */
        افعل {
            سرچینه_id = vtd_read32(i, fr_reg + 8) & SID_ماسک;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            پته[1] = vtd_read32(i, fr_reg + 4);
            پته[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_ماسک;

            چاپ_کړه("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            چاپ_کړه("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", سرچینه_id,
                   SID_BUS(سرچینه_id), SID_DEV(سرچینه_id), SID_FUNC(سرچینه_id));
            چاپ_کړه("on address 0x%x:%x ", پته[1], پته[0]);
            چاپ_کړه("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_ریکارډ_شاخص = (fault_ریکارډ_شاخص + 1) % عدد_fault_regs;
            fr_reg = fro_دننه_واټن + 16 * fault_ریکارډ_شاخص;
        } طالما ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        إذا (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

فراغ vtd_handle_fault(فراغ)
{
    drhu_id_t i;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_تېروتنې(i);
    }
}

BOOT_کوډ کلمه_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع 0;
    }
    أكد(x86KSnumIOPTLevels > 0);

    کلمه_t اندازه = 1; /* one for the root table */
    اندازه += N_VTD_CONTEXTS; /* one for each context */
    اندازه += rmrr_list->عدد; /* one for each device */

    إذا (rmrr_list->عدد == 0) {
        أرجع اندازه;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.عدد = 1;

    لكل (کلمه_t i = 1; i < rmrr_list->عدد; i++) {
        إذا (vtd_get_ریښه_شاخص(rmrr_list->entries[i].device) !=
            vtd_get_ریښه_شاخص(filtered.entries[filtered.عدد - 1].device) &&
            rmrr_list->entries[i].بنسټ != filtered.entries[filtered.عدد - 1].بنسټ &&
            rmrr_list->entries[i].حد != filtered.entries[filtered.عدد - 1].حد) {
            filtered.entries[filtered.عدد] = rmrr_list->entries[i];
            filtered.عدد++;
        }
    }

    لكل (کلمه_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا ((VTD_PT_شاخص_بټونه * i + se_L4_پاڼه_بټونه) >= 32) {
            اندازه++;
        } وإلا {
            لكل (کلمه_t j = 0; j < filtered.عدد; j++) {
                v_region_t region = (v_region_t) {
                    .پیل = filtered.entries[j].بنسټ,
                    .پای_ټکی = filtered.entries[j].حد
                };
                اندازه += get_n_paging(region, 32 - (VTD_PT_شاخص_بټونه * i + se_L4_پاڼه_بټونه));
            }
        }
    }
    أرجع اندازه;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_کوډ ساكن فراغ vtd_map_reserved_پاڼه(vtd_cte_t *vtd_context_جدول, صحيح context_شاخص, paddr_t addr)
{
    صحيح i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_خونه;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_خونه = vtd_context_جدول + context_شاخص;
    إذا (!vtd_cte_ptr_get_present(vtd_context_خونه)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_جدول_بټونه);

        *vtd_context_خونه = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_منزل_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_خونه, VTD_CTE_اندازه_بټونه);
    } وإلا {
        iopt = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_خونه));
    }
    /* now recursively find and map page tables */
    لكل (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_شاخص;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا (VTD_PT_شاخص_بټونه * i + se_L4_پاڼه_بټونه >= 32) {
            iopt_شاخص = 0;
        } وإلا {
            iopt_شاخص = ((addr >> se_L4_پاڼه_بټونه) >> (VTD_PT_شاخص_بټونه * i)) & ماسک(VTD_PT_شاخص_بټونه);
        }
        vtd_pte_خونه = iopt + iopt_شاخص;
        إذا (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_خونه = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_خونه, VTD_PTE_اندازه_بټونه);
        } وإلا {
            إذا (!vtd_pte_ptr_get_write(vtd_pte_خونه)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_جدول_بټونه);

                *vtd_pte_خونه = vtd_pte_new(pptr_منزل_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_خونه, VTD_PTE_اندازه_بټونه);
            } وإلا {
                iopt = (vtd_pte_t *)paddr_منزل_pptr(vtd_pte_ptr_get_addr(vtd_pte_خونه));
            }
        }
    }
}

BOOT_کوډ ساكن فراغ vtd_create_context_جدول(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    کلمه_t i;
    vtd_cte_t *vtd_context_جدول = (vtd_cte_t *) it_alloc_paging();

    چاپ_کړه("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_جدول);
    flushCacheRange(vtd_context_جدول, VTD_CT_اندازه_بټونه);

    x86_KSvtd_ریښه_جدول[bus] =
        vtd_rte_new(
            pptr_منزل_paddr(vtd_context_جدول), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    لكل (i = 0; i < rmrr_list->عدد; i++) {
        إذا (vtd_get_ریښه_شاخص(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            لكل (addr = rmrr_list->entries[i].بنسټ; addr < rmrr_list->entries[i].حد; addr += بټ(se_L4_پاڼه_بټونه)) {
                vtd_map_reserved_پاڼه(vtd_context_جدول, vtd_get_context_شاخص(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_کوډ ساكن bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t حالت = 0;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_منزل_paddr((فراغ *)x86_KSvtd_ریښه_جدول);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        حالت = vtd_read32(i, GSTS_REG);
        حالت |= بټ(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, حالت);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        طالما (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t معلومات, addr;

        معلومات = int_iommu;
        addr = apic_get_بنسټ_paddr();
        إذا (!addr) {
            أرجع false;
        }
        addr |= (cpu_id << 12);

        vtd_process_تېروتنې(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, معلومات);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        حالت = vtd_read32(i, GSTS_REG);
        حالت |= بټ(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, حالت);
        طالما (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        چاپ_کړه("IOMMU 0x%x: enabling...", i);

        حالت = vtd_read32(i, GSTS_REG);
        حالت |= بټ(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, حالت);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        طالما (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        چاپ_کړه(" enabled\n");
    }
    أرجع true;
}

BOOT_کوډ bool_t vtd_init_عدد_iopts(uint32_t عدد_drhu)
{
    x86KSnumDrhu = عدد_drhu;
    x86KSFirstValidIODomain = 0;

    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  عدد_domain_id_بټونه = 16;
    لكل (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t بټونه_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        چاپ_کړه("IOMMU 0x%x: %d-bit domain IDs supported\n", i, بټونه_supported);
        إذا (بټونه_supported < عدد_domain_id_بټونه) {
            عدد_domain_id_بټونه = بټونه_supported;
        }
    }

    x86KSnumIODomainIDBits = عدد_domain_id_بټونه;
    ناکارول_شوی uint32_t  max_عدد_iopt_پوړونه;
    إذا (aw_bitmask & SAGAW_6_LEVEL) {
        max_عدد_iopt_پوړونه = 6;
    } وإلا إذا (aw_bitmask & SAGAW_5_LEVEL) {
        max_عدد_iopt_پوړونه = 5;
    } وإلا إذا (aw_bitmask & SAGAW_4_LEVEL) {
        max_عدد_iopt_پوړونه = 4;
    } وإلا إذا (aw_bitmask & SAGAW_3_LEVEL) {
        max_عدد_iopt_پوړونه = 3;
    } وإلا إذا (aw_bitmask & SAGAW_2_LEVEL) {
        max_عدد_iopt_پوړونه = 2;
    } وإلا {
        چاپ_کړه("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        أرجع false;
    }

    إذا (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } وإلا إذا (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } وإلا إذا (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } وإلا إذا (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } وإلا إذا (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } وإلا {
        چاپ_کړه("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        أرجع false;
    }

    چاپ_کړه("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_عدد_iopt_پوړونه);
    أرجع true;
}


BOOT_کوډ bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    x86_KSvtd_ریښه_جدول = (vtd_rte_t *) it_alloc_paging();
    لكل (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_جدول(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_ریښه_جدول, VTD_RT_اندازه_بټونه);

    إذا (!vtd_enable(cpu_id)) {
        أرجع false;
    }
    أرجع true;
}

#د_که_پای /* CONFIG_IOMMU */
