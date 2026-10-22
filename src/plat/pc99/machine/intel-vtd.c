/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_IOMMU

#समावेश <kernel/boot.h>
#समावेश <machine.h>
#समावेश <machine/io.h>
#समावेश <arch/kernel/apic.h>
#समावेश <arch/model/statedata.h>
#समावेश <linker.h>
#समावेश <plat/machine/acpi.h>
#समावेश <plat/machine/intel-vtd.h>
#समावेश <util.h>

#घोषणा RTADDR_REG  0x20
#घोषणा GCMD_REG    0x18
#घोषणा GSTS_REG    0x1C
#घोषणा CCMD_REG    0x28
#घोषणा ECAP_REG    0x10
#घोषणा IOTLB_REG   0x08
#घोषणा FSTS_REG    0x34
#घोषणा FECTL_REG   0x38
#घोषणा FEDATA_REG  0x3C
#घोषणा FEADDR_REG  0x40
#घोषणा FEUADDR_REG 0x44
#घोषणा CAP_REG     0x08

/* Bit Positions within Registers */
#घोषणा SRTP        30  /* Set Root Table Pointer */
#घोषणा RTPS        30  /* Root Table Pointer Status */
#घोषणा TE          31  /* Translation Enable */
#घोषणा TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#घोषणा ICC         (31 + 32)  /* Invalidate Context Cache */
#घोषणा CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#घोषणा CAIG        27  /* Context Actual Invalidation Granularity */
#घोषणा CAIG_MASK   0x3
#घोषणा IVO_MASK    0x3FF
#घोषणा IVT         31  /* Invalidate IOTLB */
#घोषणा IIRG        28  /* IOTLB Invalidation Request Granularity */
#घोषणा IAIG        25  /* IOTLB Actual Invalidation Granularity */
#घोषणा IAIG_MASK   0x7
#घोषणा IP          30  /* Interrupt Pending */
#घोषणा FRI         0x8 /* Fault Recording Index */
#घोषणा FRI_MASK    0xFF
#घोषणा FRO         24
#घोषणा FRO_MASK    0xFF
#घोषणा FI          12
#घोषणा SID_MASK    0xFFFF
#घोषणा SID_BUS(a)  (MASK(8) & (a >> 8))
#घोषणा SID_DEV(a)  (MASK(5) & (a >> 3))
#घोषणा SID_FUNC(a) (MASK(3) & a)
#घोषणा FR_MASK     0xFF
#घोषणा FAULT_TYPE  30
#घोषणा FAULT       31
#घोषणा NFR         8   /* high word of CAP_REG */
#घोषणा NFR_MASK    0xff
#घोषणा PPF         1
#घोषणा PPF_MASK    1
#घोषणा PRESENT     1
#घोषणा WBF         27
#घोषणा WBFS        27
#घोषणा DID         8
#घोषणा RW          0x3

#घोषणा SAGAW         8
#घोषणा SAGAW_2_LEVEL 0x01
#घोषणा SAGAW_3_LEVEL 0x02
#घोषणा SAGAW_4_LEVEL 0x04
#घोषणा SAGAW_5_LEVEL 0x08
#घोषणा SAGAW_6_LEVEL 0x10

#घोषणा CONTEXT_GLOBAL_INVALIDATE 0x1
#घोषणा IOTLB_GLOBAL_INVALIDATE   0x1

#घोषणा DMA_TLB_READ_DRAIN  BIT(17)
#घोषणा DMA_TLB_WRITE_DRAIN BIT(16)

#घोषणा N_VTD_CONTEXTS 256

प्रकार uint32_t drhu_id_t;

अटल अंतरभूत uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t offset)
{
    वापस *(अस्थिर uint32_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset);
}

अटल अंतरभूत व्योम vtd_write32(drhu_id_t drhu_id, uint32_t offset, uint32_t मूल्य)
{
    *(अस्थिर uint32_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset) = मूल्य;
}


अटल अंतरभूत uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t offset)
{
    वापस *(अस्थिर uint64_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset);
}

अटल अंतरभूत व्योम vtd_write64(drhu_id_t drhu_id, uint32_t offset, uint64_t मूल्य)
{
    *(अस्थिर uint64_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset) = मूल्य;
}

अटल अंतरभूत uint32_t get_ivo(drhu_id_t drhu_id)
{
    वापस ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_MASK) * 16;
}

अटल uint32_t get_fro_offset(drhu_id_t drhu_id)
{
    uint32_t fro_offset;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_offset = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_MASK;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_offset |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    वापस fro_offset << 4;
}

व्योम invalidate_context_cache(व्योम)
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

    क्रम (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        जबतक ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        जबतक ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

व्योम invalidate_iotlb(व्योम)
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
    uint32_t  iotlb_reg_upper;
    uint32_t  ivo_offset;
    drhu_id_t i;

    क्रम (i = 0; i < x86KSnumDrhu; i++) {
        ivo_offset = get_ivo(i);

        /* Wait till IVT bit is clear */
        जबतक ((vtd_read32(i, ivo_offset + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_upper = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_upper |= BIT(IVT);
        iotlb_reg_upper |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_offset + IOTLB_REG, 0);
        vtd_write32(i, ivo_offset + IOTLB_REG + 4, iotlb_reg_upper);

        /* Wait for the invalidation to complete */
        जबतक ((vtd_read32(i, ivo_offset + IOTLB_REG + 4) >> IVT) & 1);
    }
}

अटल व्योम vtd_clear_fault(drhu_id_t i, word_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, BIT(FAULT));
}

अटल व्योम vtd_process_faults(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_offset;
    uint32_t source_id UNUSED;
    uint32_t fault_type UNUSED;
    uint32_t address[2] UNUSED;
    uint32_t reason UNUSED;
    uint32_t अंक_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_status;
    uint32_t fault_record_सूचकांक;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_offset = get_fro_offset(i);
    fault_status = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_MASK;

    अगर (fault_status) {
        अंक_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_MASK) + 1;
        fault_record_सूचकांक = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_MASK;
        fr_reg = fro_offset + 16 * fault_record_सूचकांक;

        /* Traverse the fault register ring buffer */
        करो {
            source_id = vtd_read32(i, fr_reg + 8) & SID_MASK;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            address[1] = vtd_read32(i, fr_reg + 4);
            address[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_MASK;

            म_लिखो("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            म_लिखो("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", source_id,
                   SID_BUS(source_id), SID_DEV(source_id), SID_FUNC(source_id));
            म_लिखो("on address 0x%x:%x ", address[1], address[0]);
            म_लिखो("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_record_सूचकांक = (fault_record_सूचकांक + 1) % अंक_fault_regs;
            fr_reg = fro_offset + 16 * fault_record_सूचकांक;
        } जबतक ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        अगर (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

व्योम vtd_handle_fault(व्योम)
{
    drhu_id_t i;

    क्रम (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_faults(i);
    }
}

BOOT_CODE word_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    अगर (x86KSnumDrhu == 0) {
        वापस 0;
    }
    निश्चित(x86KSnumIOPTLevels > 0);

    word_t आकार = 1; /* one for the root table */
    आकार += N_VTD_CONTEXTS; /* one for each context */
    आकार += rmrr_list->अंक; /* one for each device */

    अगर (rmrr_list->अंक == 0) {
        वापस आकार;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.अंक = 1;

    क्रम (word_t i = 1; i < rmrr_list->अंक; i++) {
        अगर (vtd_get_मूल_सूचकांक(rmrr_list->entries[i].device) !=
            vtd_get_मूल_सूचकांक(filtered.entries[filtered.अंक - 1].device) &&
            rmrr_list->entries[i].base != filtered.entries[filtered.अंक - 1].base &&
            rmrr_list->entries[i].limit != filtered.entries[filtered.अंक - 1].limit) {
            filtered.entries[filtered.अंक] = rmrr_list->entries[i];
            filtered.अंक++;
        }
    }

    क्रम (word_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        अगर ((VTD_PT_सूचकांक_BITS * i + seL4_PageBits) >= 32) {
            आकार++;
        } अन्यथा {
            क्रम (word_t j = 0; j < filtered.अंक; j++) {
                v_region_t region = (v_region_t) {
                    .start = filtered.entries[j].base,
                    .end = filtered.entries[j].limit
                };
                आकार += get_n_paging(region, 32 - (VTD_PT_सूचकांक_BITS * i + seL4_PageBits));
            }
        }
    }
    वापस आकार;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_CODE अटल व्योम vtd_map_reserved_page(vtd_cte_t *vtd_context_table, पूर्णांक context_सूचकांक, paddr_t addr)
{
    पूर्णांक i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_slot;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_slot = vtd_context_table + context_सूचकांक;
    अगर (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, seL4_IOPageTableBits);

        *vtd_context_slot = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_to_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_slot, VTD_CTE_आकार_BITS);
    } अन्यथा {
        iopt = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
    }
    /* now recursively find and map page tables */
    क्रम (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_सूचकांक;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        अगर (VTD_PT_सूचकांक_BITS * i + seL4_PageBits >= 32) {
            iopt_सूचकांक = 0;
        } अन्यथा {
            iopt_सूचकांक = ((addr >> seL4_PageBits) >> (VTD_PT_सूचकांक_BITS * i)) & MASK(VTD_PT_सूचकांक_BITS);
        }
        vtd_pte_slot = iopt + iopt_सूचकांक;
        अगर (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_slot = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_slot, VTD_PTE_आकार_BITS);
        } अन्यथा {
            अगर (!vtd_pte_ptr_get_write(vtd_pte_slot)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, seL4_IOPageTableBits);

                *vtd_pte_slot = vtd_pte_new(pptr_to_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_slot, VTD_PTE_आकार_BITS);
            } अन्यथा {
                iopt = (vtd_pte_t *)paddr_to_pptr(vtd_pte_ptr_get_addr(vtd_pte_slot));
            }
        }
    }
}

BOOT_CODE अटल व्योम vtd_create_context_table(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    word_t i;
    vtd_cte_t *vtd_context_table = (vtd_cte_t *) it_alloc_paging();

    म_लिखो("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_table);
    flushCacheRange(vtd_context_table, VTD_CT_आकार_BITS);

    x86_KSvtd_मूल_Table[bus] =
        vtd_rte_new(
            pptr_to_paddr(vtd_context_table), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    क्रम (i = 0; i < rmrr_list->अंक; i++) {
        अगर (vtd_get_मूल_सूचकांक(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            क्रम (addr = rmrr_list->entries[i].base; addr < rmrr_list->entries[i].limit; addr += BIT(seL4_PageBits)) {
                vtd_map_reserved_page(vtd_context_table, vtd_get_context_सूचकांक(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_CODE अटल bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t status = 0;

    क्रम (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_to_paddr((व्योम *)x86_KSvtd_मूल_Table);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        status = vtd_read32(i, GSTS_REG);
        status |= BIT(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, status);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        जबतक (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    क्रम (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t तथ्य, addr;

        तथ्य = int_iommu;
        addr = apic_get_base_paddr();
        अगर (!addr) {
            वापस false;
        }
        addr |= (cpu_id << 12);

        vtd_process_faults(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, तथ्य);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        status = vtd_read32(i, GSTS_REG);
        status |= BIT(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, status);
        जबतक (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        म_लिखो("IOMMU 0x%x: enabling...", i);

        status = vtd_read32(i, GSTS_REG);
        status |= BIT(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, status);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        जबतक (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        म_लिखो(" enabled\n");
    }
    वापस true;
}

BOOT_CODE bool_t vtd_init_अंक_iopts(uint32_t अंक_drhu)
{
    x86KSnumDrhu = अंक_drhu;
    x86KSFirstValidIODomain = 0;

    अगर (x86KSnumDrhu == 0) {
        वापस true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  अंक_domain_id_bits = 16;
    क्रम (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t bits_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        म_लिखो("IOMMU 0x%x: %d-bit domain IDs supported\n", i, bits_supported);
        अगर (bits_supported < अंक_domain_id_bits) {
            अंक_domain_id_bits = bits_supported;
        }
    }

    x86KSnumIODomainIDBits = अंक_domain_id_bits;
    UNUSED uint32_t  max_अंक_iopt_levels;
    अगर (aw_bitmask & SAGAW_6_LEVEL) {
        max_अंक_iopt_levels = 6;
    } अन्यथा अगर (aw_bitmask & SAGAW_5_LEVEL) {
        max_अंक_iopt_levels = 5;
    } अन्यथा अगर (aw_bitmask & SAGAW_4_LEVEL) {
        max_अंक_iopt_levels = 4;
    } अन्यथा अगर (aw_bitmask & SAGAW_3_LEVEL) {
        max_अंक_iopt_levels = 3;
    } अन्यथा अगर (aw_bitmask & SAGAW_2_LEVEL) {
        max_अंक_iopt_levels = 2;
    } अन्यथा {
        म_लिखो("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        वापस false;
    }

    अगर (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } अन्यथा अगर (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } अन्यथा अगर (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } अन्यथा अगर (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } अन्यथा अगर (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } अन्यथा {
        म_लिखो("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        वापस false;
    }

    म_लिखो("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_अंक_iopt_levels);
    वापस true;
}


BOOT_CODE bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    अगर (x86KSnumDrhu == 0) {
        वापस true;
    }

    x86_KSvtd_मूल_Table = (vtd_rte_t *) it_alloc_paging();
    क्रम (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_table(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_मूल_Table, VTD_RT_आकार_BITS);

    अगर (!vtd_enable(cpu_id)) {
        वापस false;
    }
    वापस true;
}

#पूर्ण_अगर /* CONFIG_IOMMU */
