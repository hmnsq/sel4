/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_IOMMU

#অন্তর্ভাব <kernel/boot.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <arch/kernel/apic.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/acpi.h>
#অন্তর্ভাব <plat/machine/intel-vtd.h>
#অন্তর্ভাব <util.h>

#define RTADDR_REG  0x20
#define GCMD_REG    0x18
#define GSTS_REG    0x1C
#define CCMD_REG    0x28
#define ECAP_REG    0x10
#define IOTLB_REG   0x08
#define FSTS_REG    0x34
#define FECTL_REG   0x38
#define FEDATA_REG  0x3C
#define FEADDR_REG  0x40
#define FEUADDR_REG 0x44
#define CAP_REG     0x08

/* Bit Positions within Registers */
#define SRTP        30  /* Set Root Table Pointer */
#define RTPS        30  /* Root Table Pointer Status */
#define TE          31  /* Translation Enable */
#define TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#define ICC         (31 + 32)  /* Invalidate Context Cache */
#define CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#define CAIG        27  /* Context Actual Invalidation Granularity */
#define CAIG_MASK   0x3
#define IVO_MASK    0x3FF
#define IVT         31  /* Invalidate IOTLB */
#define IIRG        28  /* IOTLB Invalidation Request Granularity */
#define IAIG        25  /* IOTLB Actual Invalidation Granularity */
#define IAIG_MASK   0x7
#define IP          30  /* Interrupt Pending */
#define FRI         0x8 /* Fault Recording Index */
#define FRI_MASK    0xFF
#define FRO         24
#define FRO_MASK    0xFF
#define FI          12
#define SID_MASK    0xFFFF
#define SID_BUS(a)  (MASK(8) & (a >> 8))
#define SID_DEV(a)  (MASK(5) & (a >> 3))
#define SID_FUNC(a) (MASK(3) & a)
#define FR_MASK     0xFF
#define FAULT_TYPE  30
#define FAULT       31
#define NFR         8   /* high word of CAP_REG */
#define NFR_MASK    0xff
#define PPF         1
#define PPF_MASK    1
#define PRESENT     1
#define WBF         27
#define WBFS        27
#define DID         8
#define RW          0x3

#define SAGAW         8
#define SAGAW_2_LEVEL 0x01
#define SAGAW_3_LEVEL 0x02
#define SAGAW_4_LEVEL 0x04
#define SAGAW_5_LEVEL 0x08
#define SAGAW_6_LEVEL 0x10

#define CONTEXT_GLOBAL_INVALIDATE 0x1
#define IOTLB_GLOBAL_INVALIDATE   0x1

#define DMA_TLB_READ_DRAIN  BIT(17)
#define DMA_TLB_WRITE_DRAIN BIT(16)

#define N_VTD_CONTEXTS 256

প্রকারলেখ uint32_t drhu_id_t;

স্থির অন্তঃরেখ uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t offset)
{
    ফেরত *(অস্থির uint32_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset);
}

স্থির অন্তঃরেখ শূন্য vtd_write32(drhu_id_t drhu_id, uint32_t offset, uint32_t মান)
{
    *(অস্থির uint32_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset) = মান;
}


স্থির অন্তঃরেখ uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t offset)
{
    ফেরত *(অস্থির uint64_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset);
}

স্থির অন্তঃরেখ শূন্য vtd_write64(drhu_id_t drhu_id, uint32_t offset, uint64_t মান)
{
    *(অস্থির uint64_t *)(PPTR_DRHU_START + (drhu_id << PAGE_BITS) + offset) = মান;
}

স্থির অন্তঃরেখ uint32_t get_ivo(drhu_id_t drhu_id)
{
    ফেরত ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_MASK) * 16;
}

স্থির uint32_t get_fro_offset(drhu_id_t drhu_id)
{
    uint32_t fro_offset;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_offset = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_MASK;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_offset |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    ফেরত fro_offset << 4;
}

শূন্য invalidate_context_cache(শূন্য)
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

    জন্য (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        যখন ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        যখন ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

শূন্য invalidate_iotlb(শূন্য)
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

    জন্য (i = 0; i < x86KSnumDrhu; i++) {
        ivo_offset = get_ivo(i);

        /* Wait till IVT bit is clear */
        যখন ((vtd_read32(i, ivo_offset + IOTLB_REG + 4) >> IVT) & 1);

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
        যখন ((vtd_read32(i, ivo_offset + IOTLB_REG + 4) >> IVT) & 1);
    }
}

স্থির শূন্য vtd_clear_fault(drhu_id_t i, word_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, BIT(FAULT));
}

স্থির শূন্য vtd_process_faults(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_offset;
    uint32_t source_id UNUSED;
    uint32_t fault_type UNUSED;
    uint32_t address[2] UNUSED;
    uint32_t reason UNUSED;
    uint32_t অঙ্ক_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_status;
    uint32_t fault_record_সূচক;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_offset = get_fro_offset(i);
    fault_status = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_MASK;

    যদি (fault_status) {
        অঙ্ক_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_MASK) + 1;
        fault_record_সূচক = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_MASK;
        fr_reg = fro_offset + 16 * fault_record_সূচক;

        /* Traverse the fault register ring buffer */
        করো {
            source_id = vtd_read32(i, fr_reg + 8) & SID_MASK;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            address[1] = vtd_read32(i, fr_reg + 4);
            address[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_MASK;

            printf("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            printf("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", source_id,
                   SID_BUS(source_id), SID_DEV(source_id), SID_FUNC(source_id));
            printf("on address 0x%x:%x ", address[1], address[0]);
            printf("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_record_সূচক = (fault_record_সূচক + 1) % অঙ্ক_fault_regs;
            fr_reg = fro_offset + 16 * fault_record_সূচক;
        } যখন ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        যদি (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

শূন্য vtd_handle_fault(শূন্য)
{
    drhu_id_t i;

    জন্য (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_faults(i);
    }
}

BOOT_CODE word_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    যদি (x86KSnumDrhu == 0) {
        ফেরত 0;
    }
    প্রতিজ্ঞা(x86KSnumIOPTLevels > 0);

    word_t আকার_2 = 1; /* one for the root table */
    আকার_2 += N_VTD_CONTEXTS; /* one for each context */
    আকার_2 += rmrr_list->অঙ্ক; /* one for each device */

    যদি (rmrr_list->অঙ্ক == 0) {
        ফেরত আকার_2;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.অঙ্ক = 1;

    জন্য (word_t i = 1; i < rmrr_list->অঙ্ক; i++) {
        যদি (vtd_get_মূল_সূচক(rmrr_list->entries[i].device) !=
            vtd_get_মূল_সূচক(filtered.entries[filtered.অঙ্ক - 1].device) &&
            rmrr_list->entries[i].base != filtered.entries[filtered.অঙ্ক - 1].base &&
            rmrr_list->entries[i].limit != filtered.entries[filtered.অঙ্ক - 1].limit) {
            filtered.entries[filtered.অঙ্ক] = rmrr_list->entries[i];
            filtered.অঙ্ক++;
        }
    }

    জন্য (word_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        যদি ((VTD_PT_সূচক_BITS * i + seL4_PageBits) >= 32) {
            আকার_2++;
        } অন্যথা {
            জন্য (word_t j = 0; j < filtered.অঙ্ক; j++) {
                v_region_t region = (v_region_t) {
                    .start = filtered.entries[j].base,
                    .end = filtered.entries[j].limit
                };
                আকার_2 += get_n_paging(region, 32 - (VTD_PT_সূচক_BITS * i + seL4_PageBits));
            }
        }
    }
    ফেরত আকার_2;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_CODE স্থির শূন্য vtd_map_reserved_page(vtd_cte_t *vtd_context_table, পূর্ণসংখ্যা context_সূচক, paddr_t addr)
{
    পূর্ণসংখ্যা i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_slot;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_slot = vtd_context_table + context_সূচক;
    যদি (!vtd_cte_ptr_get_present(vtd_context_slot)) {
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
        flushCacheRange(vtd_context_slot, VTD_CTE_আকার_BITS);
    } অন্যথা {
        iopt = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
    }
    /* now recursively find and map page tables */
    জন্য (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_সূচক;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        যদি (VTD_PT_সূচক_BITS * i + seL4_PageBits >= 32) {
            iopt_সূচক = 0;
        } অন্যথা {
            iopt_সূচক = ((addr >> seL4_PageBits) >> (VTD_PT_সূচক_BITS * i)) & MASK(VTD_PT_সূচক_BITS);
        }
        vtd_pte_slot = iopt + iopt_সূচক;
        যদি (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_slot = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_slot, VTD_PTE_আকার_BITS);
        } অন্যথা {
            যদি (!vtd_pte_ptr_get_write(vtd_pte_slot)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, seL4_IOPageTableBits);

                *vtd_pte_slot = vtd_pte_new(pptr_to_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_slot, VTD_PTE_আকার_BITS);
            } অন্যথা {
                iopt = (vtd_pte_t *)paddr_to_pptr(vtd_pte_ptr_get_addr(vtd_pte_slot));
            }
        }
    }
}

BOOT_CODE স্থির শূন্য vtd_create_context_table(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    word_t i;
    vtd_cte_t *vtd_context_table = (vtd_cte_t *) it_alloc_paging();

    printf("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_table);
    flushCacheRange(vtd_context_table, VTD_CT_আকার_BITS);

    x86_KSvtd_মূল_Table[bus] =
        vtd_rte_new(
            pptr_to_paddr(vtd_context_table), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    জন্য (i = 0; i < rmrr_list->অঙ্ক; i++) {
        যদি (vtd_get_মূল_সূচক(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            জন্য (addr = rmrr_list->entries[i].base; addr < rmrr_list->entries[i].limit; addr += BIT(seL4_PageBits)) {
                vtd_map_reserved_page(vtd_context_table, vtd_get_context_সূচক(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_CODE স্থির bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t status = 0;

    জন্য (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_to_paddr((শূন্য *)x86_KSvtd_মূল_Table);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        status = vtd_read32(i, GSTS_REG);
        status |= BIT(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, status);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        যখন (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    জন্য (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t তথ্য, addr;

        তথ্য = int_iommu;
        addr = apic_get_base_paddr();
        যদি (!addr) {
            ফেরত false;
        }
        addr |= (cpu_id << 12);

        vtd_process_faults(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, তথ্য);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        status = vtd_read32(i, GSTS_REG);
        status |= BIT(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, status);
        যখন (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        printf("IOMMU 0x%x: enabling...", i);

        status = vtd_read32(i, GSTS_REG);
        status |= BIT(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, status);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        যখন (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        printf(" enabled\n");
    }
    ফেরত true;
}

BOOT_CODE bool_t vtd_init_অঙ্ক_iopts(uint32_t অঙ্ক_drhu)
{
    x86KSnumDrhu = অঙ্ক_drhu;
    x86KSFirstValidIODomain = 0;

    যদি (x86KSnumDrhu == 0) {
        ফেরত true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  অঙ্ক_domain_id_bits = 16;
    জন্য (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t bits_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        printf("IOMMU 0x%x: %d-bit domain IDs supported\n", i, bits_supported);
        যদি (bits_supported < অঙ্ক_domain_id_bits) {
            অঙ্ক_domain_id_bits = bits_supported;
        }
    }

    x86KSnumIODomainIDBits = অঙ্ক_domain_id_bits;
    UNUSED uint32_t  max_অঙ্ক_iopt_levels;
    যদি (aw_bitmask & SAGAW_6_LEVEL) {
        max_অঙ্ক_iopt_levels = 6;
    } অন্যথা যদি (aw_bitmask & SAGAW_5_LEVEL) {
        max_অঙ্ক_iopt_levels = 5;
    } অন্যথা যদি (aw_bitmask & SAGAW_4_LEVEL) {
        max_অঙ্ক_iopt_levels = 4;
    } অন্যথা যদি (aw_bitmask & SAGAW_3_LEVEL) {
        max_অঙ্ক_iopt_levels = 3;
    } অন্যথা যদি (aw_bitmask & SAGAW_2_LEVEL) {
        max_অঙ্ক_iopt_levels = 2;
    } অন্যথা {
        printf("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        ফেরত false;
    }

    যদি (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } অন্যথা যদি (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } অন্যথা যদি (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } অন্যথা যদি (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } অন্যথা যদি (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } অন্যথা {
        printf("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        ফেরত false;
    }

    printf("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_অঙ্ক_iopt_levels);
    ফেরত true;
}


BOOT_CODE bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    যদি (x86KSnumDrhu == 0) {
        ফেরত true;
    }

    x86_KSvtd_মূল_Table = (vtd_rte_t *) it_alloc_paging();
    জন্য (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_table(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_মূল_Table, VTD_RT_আকার_BITS);

    যদি (!vtd_enable(cpu_id)) {
        ফেরত false;
    }
    ফেরত true;
}

#endif /* CONFIG_IOMMU */
