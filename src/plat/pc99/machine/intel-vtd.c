/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_IOMMU

#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/kernel/apic.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/acpi.h>
#अन्तर्भाव <plat/machine/intel-vtd.h>
#अन्तर्भाव <util.h>

#परिभाषय RTADDR_REG  0x20
#परिभाषय GCMD_REG    0x18
#परिभाषय GSTS_REG    0x1C
#परिभाषय CCMD_REG    0x28
#परिभाषय ECAP_REG    0x10
#परिभाषय IOTLB_REG   0x08
#परिभाषय FSTS_REG    0x34
#परिभाषय FECTL_REG   0x38
#परिभाषय FEDATA_REG  0x3C
#परिभाषय FEADDR_REG  0x40
#परिभाषय FEUADDR_REG 0x44
#परिभाषय CAP_REG     0x08

/* Bit Positions within Registers */
#परिभाषय SRTP        30  /* Set Root Table Pointer */
#परिभाषय RTPS        30  /* Root Table Pointer Status */
#परिभाषय TE          31  /* Translation Enable */
#परिभाषय TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#परिभाषय ICC         (31 + 32)  /* Invalidate Context Cache */
#परिभाषय CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#परिभाषय CAIG        27  /* Context Actual Invalidation Granularity */
#परिभाषय CAIG_आवरणम्   0x3
#परिभाषय IVO_आवरणम्    0x3FF
#परिभाषय IVT         31  /* Invalidate IOTLB */
#परिभाषय IIRG        28  /* IOTLB Invalidation Request Granularity */
#परिभाषय IAIG        25  /* IOTLB Actual Invalidation Granularity */
#परिभाषय IAIG_आवरणम्   0x7
#परिभाषय IP          30  /* Interrupt Pending */
#परिभाषय FRI         0x8 /* Fault Recording Index */
#परिभाषय FRI_आवरणम्    0xFF
#परिभाषय FRO         24
#परिभाषय FRO_आवरणम्    0xFF
#परिभाषय FI          12
#परिभाषय SID_आवरणम्    0xFFFF
#परिभाषय SID_BUS(a)  (आवरणम्(8) & (a >> 8))
#परिभाषय SID_DEV(a)  (आवरणम्(5) & (a >> 3))
#परिभाषय SID_FUNC(a) (आवरणम्(3) & a)
#परिभाषय FR_आवरणम्     0xFF
#परिभाषय FAULT_TYPE  30
#परिभाषय FAULT       31
#परिभाषय NFR         8   /* high word of CAP_REG */
#परिभाषय NFR_आवरणम्    0xff
#परिभाषय PPF         1
#परिभाषय PPF_आवरणम्    1
#परिभाषय PRESENT     1
#परिभाषय WBF         27
#परिभाषय WBFS        27
#परिभाषय DID         8
#परिभाषय RW          0x3

#परिभाषय SAGAW         8
#परिभाषय SAGAW_2_LEVEL 0x01
#परिभाषय SAGAW_3_LEVEL 0x02
#परिभाषय SAGAW_4_LEVEL 0x04
#परिभाषय SAGAW_5_LEVEL 0x08
#परिभाषय SAGAW_6_LEVEL 0x10

#परिभाषय CONTEXT_GLOBAL_INVALIDATE 0x1
#परिभाषय IOTLB_GLOBAL_INVALIDATE   0x1

#परिभाषय DMA_TLB_READ_DRAIN  BIT(17)
#परिभाषय DMA_TLB_WRITE_DRAIN BIT(16)

#परिभाषय N_VTD_CONTEXTS 256

प्रकारनिर्देश uint32_t drhu_id_t;

स्थिर अन्तःस्थ uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t विस्थापनम्)
{
    प्रत्यागम *(चञ्चल uint32_t *)(PPTR_DRHU_START + (drhu_id << PAGE_द्वयङ्काः) + विस्थापनम्);
}

स्थिर अन्तःस्थ शून्य vtd_write32(drhu_id_t drhu_id, uint32_t विस्थापनम्, uint32_t मानम्)
{
    *(चञ्चल uint32_t *)(PPTR_DRHU_START + (drhu_id << PAGE_द्वयङ्काः) + विस्थापनम्) = मानम्;
}


स्थिर अन्तःस्थ uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t विस्थापनम्)
{
    प्रत्यागम *(चञ्चल uint64_t *)(PPTR_DRHU_START + (drhu_id << PAGE_द्वयङ्काः) + विस्थापनम्);
}

स्थिर अन्तःस्थ शून्य vtd_write64(drhu_id_t drhu_id, uint32_t विस्थापनम्, uint64_t मानम्)
{
    *(चञ्चल uint64_t *)(PPTR_DRHU_START + (drhu_id << PAGE_द्वयङ्काः) + विस्थापनम्) = मानम्;
}

स्थिर अन्तःस्थ uint32_t get_ivo(drhu_id_t drhu_id)
{
    प्रत्यागम ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_आवरणम्) * 16;
}

स्थिर uint32_t get_fro_विस्थापनम्(drhu_id_t drhu_id)
{
    uint32_t fro_विस्थापनम्;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_विस्थापनम् = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_आवरणम्;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_विस्थापनम् |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    प्रत्यागम fro_विस्थापनम् << 4;
}

शून्य invalidate_context_cache(शून्य)
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

    प्रति (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        यावत् ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        यावत् ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

शून्य invalidate_iotlb(शून्य)
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
    uint32_t  iotlb_reg_ऊर्ध्वस्थम्;
    uint32_t  ivo_विस्थापनम्;
    drhu_id_t i;

    प्रति (i = 0; i < x86KSnumDrhu; i++) {
        ivo_विस्थापनम् = get_ivo(i);

        /* Wait till IVT bit is clear */
        यावत् ((vtd_read32(i, ivo_विस्थापनम् + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_ऊर्ध्वस्थम् = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_ऊर्ध्वस्थम् |= BIT(IVT);
        iotlb_reg_ऊर्ध्वस्थम् |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_विस्थापनम् + IOTLB_REG, 0);
        vtd_write32(i, ivo_विस्थापनम् + IOTLB_REG + 4, iotlb_reg_ऊर्ध्वस्थम्);

        /* Wait for the invalidation to complete */
        यावत् ((vtd_read32(i, ivo_विस्थापनम् + IOTLB_REG + 4) >> IVT) & 1);
    }
}

स्थिर शून्य vtd_clear_fault(drhu_id_t i, word_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, BIT(FAULT));
}

स्थिर शून्य vtd_process_दोषाः(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_विस्थापनम्;
    uint32_t source_id अप्रयुक्तम्;
    uint32_t fault_type अप्रयुक्तम्;
    uint32_t स्थानाङ्कः[2] अप्रयुक्तम्;
    uint32_t reason अप्रयुक्तम्;
    uint32_t अङ्कः_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_स्थितिः;
    uint32_t fault_लेखः_सूचकांक;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_विस्थापनम् = get_fro_विस्थापनम्(i);
    fault_स्थितिः = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_आवरणम्;

    यदि (fault_स्थितिः) {
        अङ्कः_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_आवरणम्) + 1;
        fault_लेखः_सूचकांक = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_आवरणम्;
        fr_reg = fro_विस्थापनम् + 16 * fault_लेखः_सूचकांक;

        /* Traverse the fault register ring buffer */
        कुरु {
            source_id = vtd_read32(i, fr_reg + 8) & SID_आवरणम्;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            स्थानाङ्कः[1] = vtd_read32(i, fr_reg + 4);
            स्थानाङ्कः[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_आवरणम्;

            प्रारूप_लिख("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            प्रारूप_लिख("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", source_id,
                   SID_BUS(source_id), SID_DEV(source_id), SID_FUNC(source_id));
            प्रारूप_लिख("on address 0x%x:%x ", स्थानाङ्कः[1], स्थानाङ्कः[0]);
            प्रारूप_लिख("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_लेखः_सूचकांक = (fault_लेखः_सूचकांक + 1) % अङ्कः_fault_regs;
            fr_reg = fro_विस्थापनम् + 16 * fault_लेखः_सूचकांक;
        } यावत् ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        यदि (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

शून्य vtd_handle_fault(शून्य)
{
    drhu_id_t i;

    प्रति (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_दोषाः(i);
    }
}

BOOT_संहिता word_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    यदि (x86KSnumDrhu == 0) {
        प्रत्यागम 0;
    }
    प्रतिज्ञा(x86KSnumIOPTLevels > 0);

    word_t आकार = 1; /* one for the root table */
    आकार += N_VTD_CONTEXTS; /* one for each context */
    आकार += rmrr_list->अङ्कः; /* one for each device */

    यदि (rmrr_list->अङ्कः == 0) {
        प्रत्यागम आकार;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.अङ्कः = 1;

    प्रति (word_t i = 1; i < rmrr_list->अङ्कः; i++) {
        यदि (vtd_get_मूलम्_सूचकांक(rmrr_list->entries[i].device) !=
            vtd_get_मूलम्_सूचकांक(filtered.entries[filtered.अङ्कः - 1].device) &&
            rmrr_list->entries[i].आधारः != filtered.entries[filtered.अङ्कः - 1].आधारः &&
            rmrr_list->entries[i].सीमा != filtered.entries[filtered.अङ्कः - 1].सीमा) {
            filtered.entries[filtered.अङ्कः] = rmrr_list->entries[i];
            filtered.अङ्कः++;
        }
    }

    प्रति (word_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        यदि ((VTD_PT_सूचकांक_द्वयङ्काः * i + se_L4_Page_द्वयङ्काः) >= 32) {
            आकार++;
        } अन्यथा {
            प्रति (word_t j = 0; j < filtered.अङ्कः; j++) {
                v_region_t region = (v_region_t) {
                    .start = filtered.entries[j].आधारः,
                    .अन्तः_2 = filtered.entries[j].सीमा
                };
                आकार += get_n_paging(region, 32 - (VTD_PT_सूचकांक_द्वयङ्काः * i + se_L4_Page_द्वयङ्काः));
            }
        }
    }
    प्रत्यागम आकार;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_संहिता स्थिर शून्य vtd_map_reserved_page(vtd_cte_t *vtd_context_सारणी, पूर्णाङ्क context_सूचकांक, paddr_t addr)
{
    पूर्णाङ्क i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_slot;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_slot = vtd_context_सारणी + context_सूचकांक;
    यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_सारणी_द्वयङ्काः);

        *vtd_context_slot = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_गन्तव्यम्_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_slot, VTD_CTE_आकार_द्वयङ्काः);
    } अन्यथा {
        iopt = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
    }
    /* now recursively find and map page tables */
    प्रति (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_सूचकांक;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        यदि (VTD_PT_सूचकांक_द्वयङ्काः * i + se_L4_Page_द्वयङ्काः >= 32) {
            iopt_सूचकांक = 0;
        } अन्यथा {
            iopt_सूचकांक = ((addr >> se_L4_Page_द्वयङ्काः) >> (VTD_PT_सूचकांक_द्वयङ्काः * i)) & आवरणम्(VTD_PT_सूचकांक_द्वयङ्काः);
        }
        vtd_pte_slot = iopt + iopt_सूचकांक;
        यदि (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_slot = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_slot, VTD_PTE_आकार_द्वयङ्काः);
        } अन्यथा {
            यदि (!vtd_pte_ptr_get_write(vtd_pte_slot)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_सारणी_द्वयङ्काः);

                *vtd_pte_slot = vtd_pte_new(pptr_गन्तव्यम्_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_slot, VTD_PTE_आकार_द्वयङ्काः);
            } अन्यथा {
                iopt = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_pte_ptr_get_addr(vtd_pte_slot));
            }
        }
    }
}

BOOT_संहिता स्थिर शून्य vtd_create_context_सारणी(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    word_t i;
    vtd_cte_t *vtd_context_सारणी = (vtd_cte_t *) it_alloc_paging();

    प्रारूप_लिख("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_सारणी);
    flushCacheRange(vtd_context_सारणी, VTD_CT_आकार_द्वयङ्काः);

    x86_KSvtd_मूलम्_सारणी[bus] =
        vtd_rte_new(
            pptr_गन्तव्यम्_paddr(vtd_context_सारणी), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    प्रति (i = 0; i < rmrr_list->अङ्कः; i++) {
        यदि (vtd_get_मूलम्_सूचकांक(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            प्रति (addr = rmrr_list->entries[i].आधारः; addr < rmrr_list->entries[i].सीमा; addr += BIT(se_L4_Page_द्वयङ्काः)) {
                vtd_map_reserved_page(vtd_context_सारणी, vtd_get_context_सूचकांक(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_संहिता स्थिर bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t स्थितिः = 0;

    प्रति (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_गन्तव्यम्_paddr((शून्य *)x86_KSvtd_मूलम्_सारणी);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        स्थितिः = vtd_read32(i, GSTS_REG);
        स्थितिः |= BIT(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, स्थितिः);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        यावत् (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    प्रति (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t दत्त, addr;

        दत्त = int_iommu;
        addr = apic_get_आधारः_paddr();
        यदि (!addr) {
            प्रत्यागम false;
        }
        addr |= (cpu_id << 12);

        vtd_process_दोषाः(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, दत्त);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        स्थितिः = vtd_read32(i, GSTS_REG);
        स्थितिः |= BIT(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, स्थितिः);
        यावत् (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        प्रारूप_लिख("IOMMU 0x%x: enabling...", i);

        स्थितिः = vtd_read32(i, GSTS_REG);
        स्थितिः |= BIT(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, स्थितिः);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        यावत् (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        प्रारूप_लिख(" enabled\n");
    }
    प्रत्यागम true;
}

BOOT_संहिता bool_t vtd_init_अङ्कः_iopts(uint32_t अङ्कः_drhu)
{
    x86KSnumDrhu = अङ्कः_drhu;
    x86KSFirstValidIODomain = 0;

    यदि (x86KSnumDrhu == 0) {
        प्रत्यागम true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  अङ्कः_domain_id_द्वयङ्काः = 16;
    प्रति (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t द्वयङ्काः_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        प्रारूप_लिख("IOMMU 0x%x: %d-bit domain IDs supported\n", i, द्वयङ्काः_supported);
        यदि (द्वयङ्काः_supported < अङ्कः_domain_id_द्वयङ्काः) {
            अङ्कः_domain_id_द्वयङ्काः = द्वयङ्काः_supported;
        }
    }

    x86KSnumIODomainIDBits = अङ्कः_domain_id_द्वयङ्काः;
    अप्रयुक्तम् uint32_t  max_अङ्कः_iopt_स्तराः;
    यदि (aw_bitmask & SAGAW_6_LEVEL) {
        max_अङ्कः_iopt_स्तराः = 6;
    } अन्यथा यदि (aw_bitmask & SAGAW_5_LEVEL) {
        max_अङ्कः_iopt_स्तराः = 5;
    } अन्यथा यदि (aw_bitmask & SAGAW_4_LEVEL) {
        max_अङ्कः_iopt_स्तराः = 4;
    } अन्यथा यदि (aw_bitmask & SAGAW_3_LEVEL) {
        max_अङ्कः_iopt_स्तराः = 3;
    } अन्यथा यदि (aw_bitmask & SAGAW_2_LEVEL) {
        max_अङ्कः_iopt_स्तराः = 2;
    } अन्यथा {
        प्रारूप_लिख("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        प्रत्यागम false;
    }

    यदि (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } अन्यथा यदि (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } अन्यथा यदि (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } अन्यथा यदि (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } अन्यथा यदि (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } अन्यथा {
        प्रारूप_लिख("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        प्रत्यागम false;
    }

    प्रारूप_लिख("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_अङ्कः_iopt_स्तराः);
    प्रत्यागम true;
}


BOOT_संहिता bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    यदि (x86KSnumDrhu == 0) {
        प्रत्यागम true;
    }

    x86_KSvtd_मूलम्_सारणी = (vtd_rte_t *) it_alloc_paging();
    प्रति (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_सारणी(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_मूलम्_सारणी, VTD_RT_आकार_द्वयङ्काः);

    यदि (!vtd_enable(cpu_id)) {
        प्रत्यागम false;
    }
    प्रत्यागम true;
}

#यदि_अन्तः /* CONFIG_IOMMU */
