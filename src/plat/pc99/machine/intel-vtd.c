/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU

#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/apic.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/acpi.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ವ್ಯಾಖ್ಯಾನಿಸು RTADDR_REG  0x20
#ವ್ಯಾಖ್ಯಾನಿಸು GCMD_REG    0x18
#ವ್ಯಾಖ್ಯಾನಿಸು GSTS_REG    0x1C
#ವ್ಯಾಖ್ಯಾನಿಸು CCMD_REG    0x28
#ವ್ಯಾಖ್ಯಾನಿಸು ECAP_REG    0x10
#ವ್ಯಾಖ್ಯಾನಿಸು IOTLB_REG   0x08
#ವ್ಯಾಖ್ಯಾನಿಸು FSTS_REG    0x34
#ವ್ಯಾಖ್ಯಾನಿಸು FECTL_REG   0x38
#ವ್ಯಾಖ್ಯಾನಿಸು FEDATA_REG  0x3C
#ವ್ಯಾಖ್ಯಾನಿಸು FEADDR_REG  0x40
#ವ್ಯಾಖ್ಯಾನಿಸು FEUADDR_REG 0x44
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_REG     0x08

/* Bit Positions within Registers */
#ವ್ಯಾಖ್ಯಾನಿಸು SRTP        30  /* Set Root Table Pointer */
#ವ್ಯಾಖ್ಯಾನಿಸು RTPS        30  /* Root Table Pointer Status */
#ವ್ಯಾಖ್ಯಾನಿಸು TE          31  /* Translation Enable */
#ವ್ಯಾಖ್ಯಾನಿಸು TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ICC         (31 + 32)  /* Invalidate Context Cache */
#ವ್ಯಾಖ್ಯಾನಿಸು CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#ವ್ಯಾಖ್ಯಾನಿಸು CAIG        27  /* Context Actual Invalidation Granularity */
#ವ್ಯಾಖ್ಯಾನಿಸು CAIG_ಮುಖವಾಡ   0x3
#ವ್ಯಾಖ್ಯಾನಿಸು IVO_ಮುಖವಾಡ    0x3FF
#ವ್ಯಾಖ್ಯಾನಿಸು IVT         31  /* Invalidate IOTLB */
#ವ್ಯಾಖ್ಯಾನಿಸು IIRG        28  /* IOTLB Invalidation Request Granularity */
#ವ್ಯಾಖ್ಯಾನಿಸು IAIG        25  /* IOTLB Actual Invalidation Granularity */
#ವ್ಯಾಖ್ಯಾನಿಸು IAIG_ಮುಖವಾಡ   0x7
#ವ್ಯಾಖ್ಯಾನಿಸು IP          30  /* Interrupt Pending */
#ವ್ಯಾಖ್ಯಾನಿಸು FRI         0x8 /* Fault Recording Index */
#ವ್ಯಾಖ್ಯಾನಿಸು FRI_ಮುಖವಾಡ    0xFF
#ವ್ಯಾಖ್ಯಾನಿಸು FRO         24
#ವ್ಯಾಖ್ಯಾನಿಸು FRO_ಮುಖವಾಡ    0xFF
#ವ್ಯಾಖ್ಯಾನಿಸು FI          12
#ವ್ಯಾಖ್ಯಾನಿಸು SID_ಮುಖವಾಡ    0xFFFF
#ವ್ಯಾಖ್ಯಾನಿಸು SID_BUS(a)  (ಮುಖವಾಡ(8) & (a >> 8))
#ವ್ಯಾಖ್ಯಾನಿಸು SID_DEV(a)  (ಮುಖವಾಡ(5) & (a >> 3))
#ವ್ಯಾಖ್ಯಾನಿಸು SID_FUNC(a) (ಮುಖವಾಡ(3) & a)
#ವ್ಯಾಖ್ಯಾನಿಸು FR_ಮುಖವಾಡ     0xFF
#ವ್ಯಾಖ್ಯಾನಿಸು FAULT_TYPE  30
#ವ್ಯಾಖ್ಯಾನಿಸು FAULT       31
#ವ್ಯಾಖ್ಯಾನಿಸು NFR         8   /* high word of CAP_REG */
#ವ್ಯಾಖ್ಯಾನಿಸು NFR_ಮುಖವಾಡ    0xff
#ವ್ಯಾಖ್ಯಾನಿಸು PPF         1
#ವ್ಯಾಖ್ಯಾನಿಸು PPF_ಮುಖವಾಡ    1
#ವ್ಯಾಖ್ಯಾನಿಸು PRESENT     1
#ವ್ಯಾಖ್ಯಾನಿಸು WBF         27
#ವ್ಯಾಖ್ಯಾನಿಸು WBFS        27
#ವ್ಯಾಖ್ಯಾನಿಸು DID         8
#ವ್ಯಾಖ್ಯಾನಿಸು RW          0x3

#ವ್ಯಾಖ್ಯಾನಿಸು SAGAW         8
#ವ್ಯಾಖ್ಯಾನಿಸು SAGAW_2_LEVEL 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು SAGAW_3_LEVEL 0x02
#ವ್ಯಾಖ್ಯಾನಿಸು SAGAW_4_LEVEL 0x04
#ವ್ಯಾಖ್ಯಾನಿಸು SAGAW_5_LEVEL 0x08
#ವ್ಯಾಖ್ಯಾನಿಸು SAGAW_6_LEVEL 0x10

#ವ್ಯಾಖ್ಯಾನಿಸು CONTEXT_GLOBAL_INVALIDATE 0x1
#ವ್ಯಾಖ್ಯಾನಿಸು IOTLB_GLOBAL_INVALIDATE   0x1

#ವ್ಯಾಖ್ಯಾನಿಸು DMA_TLB_READ_DRAIN  ಬಿಟ್(17)
#ವ್ಯಾಖ್ಯಾನಿಸು DMA_TLB_WRITE_DRAIN ಬಿಟ್(16)

#ವ್ಯಾಖ್ಯಾನಿಸು N_VTD_CONTEXTS 256

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t drhu_id_t;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t ಸ್ಥಾನಾಂತರ)
{
    ಹಿಂತಿರುಗಿ *(ಅಸ್ಥಿರ uint32_t *)(PPTR_DRHU_ಆರಂಭ + (drhu_id << ಪುಟ_ಬಿಟ್ಟುಗಳು) + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ vtd_write32(drhu_id_t drhu_id, uint32_t ಸ್ಥಾನಾಂತರ, uint32_t ಮೌಲ್ಯ)
{
    *(ಅಸ್ಥಿರ uint32_t *)(PPTR_DRHU_ಆರಂಭ + (drhu_id << ಪುಟ_ಬಿಟ್ಟುಗಳು) + ಸ್ಥಾನಾಂತರ) = ಮೌಲ್ಯ;
}


ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t ಸ್ಥಾನಾಂತರ)
{
    ಹಿಂತಿರುಗಿ *(ಅಸ್ಥಿರ uint64_t *)(PPTR_DRHU_ಆರಂಭ + (drhu_id << ಪುಟ_ಬಿಟ್ಟುಗಳು) + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ vtd_write64(drhu_id_t drhu_id, uint32_t ಸ್ಥಾನಾಂತರ, uint64_t ಮೌಲ್ಯ)
{
    *(ಅಸ್ಥಿರ uint64_t *)(PPTR_DRHU_ಆರಂಭ + (drhu_id << ಪುಟ_ಬಿಟ್ಟುಗಳು) + ಸ್ಥಾನಾಂತರ) = ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t get_ivo(drhu_id_t drhu_id)
{
    ಹಿಂತಿರುಗಿ ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_ಮುಖವಾಡ) * 16;
}

ಸ್ಥಿರ uint32_t get_fro_ಸ್ಥಾನಾಂತರ(drhu_id_t drhu_id)
{
    uint32_t fro_ಸ್ಥಾನಾಂತರ;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_ಸ್ಥಾನಾಂತರ = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_ಮುಖವಾಡ;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_ಸ್ಥಾನಾಂತರ |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    ಹಿಂತಿರುಗಿ fro_ಸ್ಥಾನಾಂತರ << 4;
}

ಶೂನ್ಯ invalidate_context_cache(ಶೂನ್ಯ)
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

    ಗಾಗಿ (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        ಆಗ ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        ಆಗ ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

ಶೂನ್ಯ invalidate_iotlb(ಶೂನ್ಯ)
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
    uint32_t  iotlb_reg_ಊರ್ಧ್ವ;
    uint32_t  ivo_ಸ್ಥಾನಾಂತರ;
    drhu_id_t i;

    ಗಾಗಿ (i = 0; i < x86KSnumDrhu; i++) {
        ivo_ಸ್ಥಾನಾಂತರ = get_ivo(i);

        /* Wait till IVT bit is clear */
        ಆಗ ((vtd_read32(i, ivo_ಸ್ಥಾನಾಂತರ + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_ಊರ್ಧ್ವ = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_ಊರ್ಧ್ವ |= ಬಿಟ್(IVT);
        iotlb_reg_ಊರ್ಧ್ವ |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_ಸ್ಥಾನಾಂತರ + IOTLB_REG, 0);
        vtd_write32(i, ivo_ಸ್ಥಾನಾಂತರ + IOTLB_REG + 4, iotlb_reg_ಊರ್ಧ್ವ);

        /* Wait for the invalidation to complete */
        ಆಗ ((vtd_read32(i, ivo_ಸ್ಥಾನಾಂತರ + IOTLB_REG + 4) >> IVT) & 1);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vtd_clear_fault(drhu_id_t i, ಶಬ್ದ_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, ಬಿಟ್(FAULT));
}

ಸ್ಥಿರ ಶೂನ್ಯ vtd_process_ದೋಷಗಳು(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_ಸ್ಥಾನಾಂತರ;
    uint32_t ಮೂಲ_id ಬಳಸದ;
    uint32_t fault_type ಬಳಸದ;
    uint32_t ವಿಳಾಸ[2] ಬಳಸದ;
    uint32_t reason ಬಳಸದ;
    uint32_t ಅಂಕೆ_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_ಸ್ಥಿತಿ;
    uint32_t fault_ದಾಖಲೆ_ಸೂಚ್ಯಂಕ;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_ಸ್ಥಾನಾಂತರ = get_fro_ಸ್ಥಾನಾಂತರ(i);
    fault_ಸ್ಥಿತಿ = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_ಮುಖವಾಡ;

    ಆದರೆ (fault_ಸ್ಥಿತಿ) {
        ಅಂಕೆ_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_ಮುಖವಾಡ) + 1;
        fault_ದಾಖಲೆ_ಸೂಚ್ಯಂಕ = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_ಮುಖವಾಡ;
        fr_reg = fro_ಸ್ಥಾನಾಂತರ + 16 * fault_ದಾಖಲೆ_ಸೂಚ್ಯಂಕ;

        /* Traverse the fault register ring buffer */
        ಮಾಡು {
            ಮೂಲ_id = vtd_read32(i, fr_reg + 8) & SID_ಮುಖವಾಡ;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            ವಿಳಾಸ[1] = vtd_read32(i, fr_reg + 4);
            ವಿಳಾಸ[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_ಮುಖವಾಡ;

            ಬರೆ("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            ಬರೆ("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", ಮೂಲ_id,
                   SID_BUS(ಮೂಲ_id), SID_DEV(ಮೂಲ_id), SID_FUNC(ಮೂಲ_id));
            ಬರೆ("on address 0x%x:%x ", ವಿಳಾಸ[1], ವಿಳಾಸ[0]);
            ಬರೆ("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_ದಾಖಲೆ_ಸೂಚ್ಯಂಕ = (fault_ದಾಖಲೆ_ಸೂಚ್ಯಂಕ + 1) % ಅಂಕೆ_fault_regs;
            fr_reg = fro_ಸ್ಥಾನಾಂತರ + 16 * fault_ದಾಖಲೆ_ಸೂಚ್ಯಂಕ;
        } ಆಗ ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        ಆದರೆ (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

ಶೂನ್ಯ vtd_handle_fault(ಶೂನ್ಯ)
{
    drhu_id_t i;

    ಗಾಗಿ (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_ದೋಷಗಳು(i);
    }
}

BOOT_ಸಂಕೇತ ಶಬ್ದ_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    ಆದರೆ (x86KSnumDrhu == 0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಪ್ರತಿಜ್ಞಾ(x86KSnumIOPTLevels > 0);

    ಶಬ್ದ_t ಗಾತ್ರ_2 = 1; /* one for the root table */
    ಗಾತ್ರ_2 += N_VTD_CONTEXTS; /* one for each context */
    ಗಾತ್ರ_2 += rmrr_list->ಅಂಕೆ; /* one for each device */

    ಆದರೆ (rmrr_list->ಅಂಕೆ == 0) {
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.ಅಂಕೆ = 1;

    ಗಾಗಿ (ಶಬ್ದ_t i = 1; i < rmrr_list->ಅಂಕೆ; i++) {
        ಆದರೆ (vtd_get_ಬೇರು_ಸೂಚ್ಯಂಕ(rmrr_list->entries[i].device) !=
            vtd_get_ಬೇರು_ಸೂಚ್ಯಂಕ(filtered.entries[filtered.ಅಂಕೆ - 1].device) &&
            rmrr_list->entries[i].ತಳ != filtered.entries[filtered.ಅಂಕೆ - 1].ತಳ &&
            rmrr_list->entries[i].ಎಲ್ಲೆ != filtered.entries[filtered.ಅಂಕೆ - 1].ಎಲ್ಲೆ) {
            filtered.entries[filtered.ಅಂಕೆ] = rmrr_list->entries[i];
            filtered.ಅಂಕೆ++;
        }
    }

    ಗಾಗಿ (ಶಬ್ದ_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        ಆದರೆ ((VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * i + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು) >= 32) {
            ಗಾತ್ರ_2++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಾಗಿ (ಶಬ್ದ_t j = 0; j < filtered.ಅಂಕೆ; j++) {
                v_region_t region = (v_region_t) {
                    .ಆರಂಭ = filtered.entries[j].ತಳ,
                    .ಕೊನೆ = filtered.entries[j].ಎಲ್ಲೆ
                };
                ಗಾತ್ರ_2 += get_n_paging(region, 32 - (VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * i + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು));
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ vtd_map_reserved_ಪುಟ(vtd_cte_t *vtd_context_ಕೋಷ್ಟಕ, ಪೂರ್ಣಾಂಕ context_ಸೂಚ್ಯಂಕ, paddr_t addr)
{
    ಪೂರ್ಣಾಂಕ i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_ಸ್ಥಾನ;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_ಸ್ಥಾನ = vtd_context_ಕೋಷ್ಟಕ + context_ಸೂಚ್ಯಂಕ;
    ಆದರೆ (!vtd_cte_ptr_get_present(vtd_context_ಸ್ಥಾನ)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);

        *vtd_context_ಸ್ಥಾನ = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_ಗಮ್ಯ_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_ಸ್ಥಾನ, VTD_CTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        iopt = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_cte_ptr_get_asr(vtd_context_ಸ್ಥಾನ));
    }
    /* now recursively find and map page tables */
    ಗಾಗಿ (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_ಸೂಚ್ಯಂಕ;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        ಆದರೆ (VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * i + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು >= 32) {
            iopt_ಸೂಚ್ಯಂಕ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            iopt_ಸೂಚ್ಯಂಕ = ((addr >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು) >> (VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * i)) & ಮುಖವಾಡ(VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
        }
        vtd_pte_ಸ್ಥಾನ = iopt + iopt_ಸೂಚ್ಯಂಕ;
        ಆದರೆ (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_ಸ್ಥಾನ = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_ಸ್ಥಾನ, VTD_PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (!vtd_pte_ptr_get_write(vtd_pte_ಸ್ಥಾನ)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);

                *vtd_pte_ಸ್ಥಾನ = vtd_pte_new(pptr_ಗಮ್ಯ_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_ಸ್ಥಾನ, VTD_PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
            } ಇಲ್ಲದಿದ್ದರೆ {
                iopt = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_pte_ptr_get_addr(vtd_pte_ಸ್ಥಾನ));
            }
        }
    }
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ vtd_create_context_ಕೋಷ್ಟಕ(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    ಶಬ್ದ_t i;
    vtd_cte_t *vtd_context_ಕೋಷ್ಟಕ = (vtd_cte_t *) it_alloc_paging();

    ಬರೆ("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_ಕೋಷ್ಟಕ);
    flushCacheRange(vtd_context_ಕೋಷ್ಟಕ, VTD_CT_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);

    x86_KSvtd_ಬೇರು_ಕೋಷ್ಟಕ[bus] =
        vtd_rte_new(
            pptr_ಗಮ್ಯ_paddr(vtd_context_ಕೋಷ್ಟಕ), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    ಗಾಗಿ (i = 0; i < rmrr_list->ಅಂಕೆ; i++) {
        ಆದರೆ (vtd_get_ಬೇರು_ಸೂಚ್ಯಂಕ(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            ಗಾಗಿ (addr = rmrr_list->entries[i].ತಳ; addr < rmrr_list->entries[i].ಎಲ್ಲೆ; addr += ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
                vtd_map_reserved_ಪುಟ(vtd_context_ಕೋಷ್ಟಕ, vtd_get_context_ಸೂಚ್ಯಂಕ(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t ಸ್ಥಿತಿ_2 = 0;

    ಗಾಗಿ (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)x86_KSvtd_ಬೇರು_ಕೋಷ್ಟಕ);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        ಸ್ಥಿತಿ_2 = vtd_read32(i, GSTS_REG);
        ಸ್ಥಿತಿ_2 |= ಬಿಟ್(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, ಸ್ಥಿತಿ_2);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        ಆಗ (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    ಗಾಗಿ (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t ದತ್ತಾಂಶ, addr;

        ದತ್ತಾಂಶ = int_iommu;
        addr = apic_get_ತಳ_paddr();
        ಆದರೆ (!addr) {
            ಹಿಂತಿರುಗಿ false;
        }
        addr |= (cpu_id << 12);

        vtd_process_ದೋಷಗಳು(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, ದತ್ತಾಂಶ);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        ಸ್ಥಿತಿ_2 = vtd_read32(i, GSTS_REG);
        ಸ್ಥಿತಿ_2 |= ಬಿಟ್(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, ಸ್ಥಿತಿ_2);
        ಆಗ (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        ಬರೆ("IOMMU 0x%x: enabling...", i);

        ಸ್ಥಿತಿ_2 = vtd_read32(i, GSTS_REG);
        ಸ್ಥಿತಿ_2 |= ಬಿಟ್(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, ಸ್ಥಿತಿ_2);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        ಆಗ (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        ಬರೆ(" enabled\n");
    }
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ bool_t vtd_init_ಅಂಕೆ_iopts(uint32_t ಅಂಕೆ_drhu)
{
    x86KSnumDrhu = ಅಂಕೆ_drhu;
    x86KSFirstValidIODomain = 0;

    ಆದರೆ (x86KSnumDrhu == 0) {
        ಹಿಂತಿರುಗಿ true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  ಅಂಕೆ_domain_id_ಬಿಟ್ಟುಗಳು = 16;
    ಗಾಗಿ (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t ಬಿಟ್ಟುಗಳು_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        ಬರೆ("IOMMU 0x%x: %d-bit domain IDs supported\n", i, ಬಿಟ್ಟುಗಳು_supported);
        ಆದರೆ (ಬಿಟ್ಟುಗಳು_supported < ಅಂಕೆ_domain_id_ಬಿಟ್ಟುಗಳು) {
            ಅಂಕೆ_domain_id_ಬಿಟ್ಟುಗಳು = ಬಿಟ್ಟುಗಳು_supported;
        }
    }

    x86KSnumIODomainIDBits = ಅಂಕೆ_domain_id_ಬಿಟ್ಟುಗಳು;
    ಬಳಸದ uint32_t  max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು;
    ಆದರೆ (aw_bitmask & SAGAW_6_LEVEL) {
        max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು = 6;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_5_LEVEL) {
        max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು = 5;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_4_LEVEL) {
        max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು = 4;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_3_LEVEL) {
        max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು = 3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_2_LEVEL) {
        max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು = 2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಬರೆ("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_ಅಂಕೆ_iopt_ಮಟ್ಟಗಳು);
    ಹಿಂತಿರುಗಿ true;
}


BOOT_ಸಂಕೇತ bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    ಆದರೆ (x86KSnumDrhu == 0) {
        ಹಿಂತಿರುಗಿ true;
    }

    x86_KSvtd_ಬೇರು_ಕೋಷ್ಟಕ = (vtd_rte_t *) it_alloc_paging();
    ಗಾಗಿ (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_ಕೋಷ್ಟಕ(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_ಬೇರು_ಕೋಷ್ಟಕ, VTD_RT_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);

    ಆದರೆ (!vtd_enable(cpu_id)) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_IOMMU */
