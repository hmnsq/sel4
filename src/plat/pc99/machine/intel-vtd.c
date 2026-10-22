/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU

#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/apic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/acpi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTADDR_REG  0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GCMD_REG    0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GSTS_REG    0x1C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CCMD_REG    0x28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECAP_REG    0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOTLB_REG   0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSTS_REG    0x34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FECTL_REG   0x38
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEDATA_REG  0x3C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEADDR_REG  0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEUADDR_REG 0x44
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_REG     0x08

/* Bit Positions within Registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SRTP        30  /* Set Root Table Pointer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTPS        30  /* Root Table Pointer Status */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TE          31  /* Translation Enable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC         (31 + 32)  /* Invalidate Context Cache */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAIG        27  /* Context Actual Invalidation Granularity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAIG_ਨਕਾਬ   0x3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVO_ਨਕਾਬ    0x3FF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVT         31  /* Invalidate IOTLB */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IIRG        28  /* IOTLB Invalidation Request Granularity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IAIG        25  /* IOTLB Actual Invalidation Granularity */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IAIG_ਨਕਾਬ   0x7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IP          30  /* Interrupt Pending */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRI         0x8 /* Fault Recording Index */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRI_ਨਕਾਬ    0xFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRO         24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRO_ਨਕਾਬ    0xFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FI          12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_ਨਕਾਬ    0xFFFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_BUS(a)  (ਨਕਾਬ(8) & (a >> 8))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_DEV(a)  (ਨਕਾਬ(5) & (a >> 3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SID_FUNC(a) (ਨਕਾਬ(3) & a)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FR_ਨਕਾਬ     0xFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAULT_TYPE  30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAULT       31
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFR         8   /* high word of CAP_REG */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFR_ਨਕਾਬ    0xff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PPF         1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PPF_ਨਕਾਬ    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESENT     1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WBF         27
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WBFS        27
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DID         8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RW          0x3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAGAW         8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAGAW_2_LEVEL 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAGAW_3_LEVEL 0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAGAW_4_LEVEL 0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAGAW_5_LEVEL 0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAGAW_6_LEVEL 0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONTEXT_GLOBAL_INVALIDATE 0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOTLB_GLOBAL_INVALIDATE   0x1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DMA_TLB_READ_DRAIN  ਦੋਆਧਾਰੀ_ਅੰਕ(17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DMA_TLB_WRITE_DRAIN ਦੋਆਧਾਰੀ_ਅੰਕ(16)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ N_VTD_CONTEXTS 256

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t drhu_id_t;

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t ਵਿੱਥ)
{
    ਵਾਪਸ *(ਅਸਥਿਰ uint32_t *)(PPTR_DRHU_ਸ਼ੁਰੂ + (drhu_id << ਪੰਨਾ_ਬਿੱਟ) + ਵਿੱਥ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ vtd_write32(drhu_id_t drhu_id, uint32_t ਵਿੱਥ, uint32_t ਮੁੱਲ)
{
    *(ਅਸਥਿਰ uint32_t *)(PPTR_DRHU_ਸ਼ੁਰੂ + (drhu_id << ਪੰਨਾ_ਬਿੱਟ) + ਵਿੱਥ) = ਮੁੱਲ;
}


ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t ਵਿੱਥ)
{
    ਵਾਪਸ *(ਅਸਥਿਰ uint64_t *)(PPTR_DRHU_ਸ਼ੁਰੂ + (drhu_id << ਪੰਨਾ_ਬਿੱਟ) + ਵਿੱਥ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ vtd_write64(drhu_id_t drhu_id, uint32_t ਵਿੱਥ, uint64_t ਮੁੱਲ)
{
    *(ਅਸਥਿਰ uint64_t *)(PPTR_DRHU_ਸ਼ੁਰੂ + (drhu_id << ਪੰਨਾ_ਬਿੱਟ) + ਵਿੱਥ) = ਮੁੱਲ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t get_ivo(drhu_id_t drhu_id)
{
    ਵਾਪਸ ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_ਨਕਾਬ) * 16;
}

ਸਥਿਰ uint32_t get_fro_ਵਿੱਥ(drhu_id_t drhu_id)
{
    uint32_t fro_ਵਿੱਥ;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_ਵਿੱਥ = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_ਨਕਾਬ;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_ਵਿੱਥ |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    ਵਾਪਸ fro_ਵਿੱਥ << 4;
}

ਖਾਲੀ invalidate_context_cache(ਖਾਲੀ)
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

    ਲਈ (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        ਜਦੋਂ ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        ਜਦੋਂ ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

ਖਾਲੀ invalidate_iotlb(ਖਾਲੀ)
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
    uint32_t  iotlb_reg_ਉੱਤਲਾ;
    uint32_t  ivo_ਵਿੱਥ;
    drhu_id_t i;

    ਲਈ (i = 0; i < x86KSnumDrhu; i++) {
        ivo_ਵਿੱਥ = get_ivo(i);

        /* Wait till IVT bit is clear */
        ਜਦੋਂ ((vtd_read32(i, ivo_ਵਿੱਥ + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_ਉੱਤਲਾ = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_ਉੱਤਲਾ |= ਦੋਆਧਾਰੀ_ਅੰਕ(IVT);
        iotlb_reg_ਉੱਤਲਾ |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_ਵਿੱਥ + IOTLB_REG, 0);
        vtd_write32(i, ivo_ਵਿੱਥ + IOTLB_REG + 4, iotlb_reg_ਉੱਤਲਾ);

        /* Wait for the invalidation to complete */
        ਜਦੋਂ ((vtd_read32(i, ivo_ਵਿੱਥ + IOTLB_REG + 4) >> IVT) & 1);
    }
}

ਸਥਿਰ ਖਾਲੀ vtd_clear_fault(drhu_id_t i, ਸ਼ਬਦ_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, ਦੋਆਧਾਰੀ_ਅੰਕ(FAULT));
}

ਸਥਿਰ ਖਾਲੀ vtd_process_ਨੁਕਸ(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_ਵਿੱਥ;
    uint32_t ਸਰੋਤ_id ਅਣਵਰਤਿਆ;
    uint32_t fault_type ਅਣਵਰਤਿਆ;
    uint32_t ਪਤਾ[2] ਅਣਵਰਤਿਆ;
    uint32_t reason ਅਣਵਰਤਿਆ;
    uint32_t ਅੰਕ_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_ਸਥਿਤੀ;
    uint32_t fault_ਰਿਕਾਰਡ_ਸੂਚਕਾਂਕ;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_ਵਿੱਥ = get_fro_ਵਿੱਥ(i);
    fault_ਸਥਿਤੀ = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_ਨਕਾਬ;

    ਜੇ (fault_ਸਥਿਤੀ) {
        ਅੰਕ_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_ਨਕਾਬ) + 1;
        fault_ਰਿਕਾਰਡ_ਸੂਚਕਾਂਕ = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_ਨਕਾਬ;
        fr_reg = fro_ਵਿੱਥ + 16 * fault_ਰਿਕਾਰਡ_ਸੂਚਕਾਂਕ;

        /* Traverse the fault register ring buffer */
        ਕਰੋ {
            ਸਰੋਤ_id = vtd_read32(i, fr_reg + 8) & SID_ਨਕਾਬ;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            ਪਤਾ[1] = vtd_read32(i, fr_reg + 4);
            ਪਤਾ[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_ਨਕਾਬ;

            ਲਿਖੋ("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            ਲਿਖੋ("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", ਸਰੋਤ_id,
                   SID_BUS(ਸਰੋਤ_id), SID_DEV(ਸਰੋਤ_id), SID_FUNC(ਸਰੋਤ_id));
            ਲਿਖੋ("on address 0x%x:%x ", ਪਤਾ[1], ਪਤਾ[0]);
            ਲਿਖੋ("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_ਰਿਕਾਰਡ_ਸੂਚਕਾਂਕ = (fault_ਰਿਕਾਰਡ_ਸੂਚਕਾਂਕ + 1) % ਅੰਕ_fault_regs;
            fr_reg = fro_ਵਿੱਥ + 16 * fault_ਰਿਕਾਰਡ_ਸੂਚਕਾਂਕ;
        } ਜਦੋਂ ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        ਜੇ (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

ਖਾਲੀ vtd_handle_fault(ਖਾਲੀ)
{
    drhu_id_t i;

    ਲਈ (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_ਨੁਕਸ(i);
    }
}

BOOT_ਕੋਡ ਸ਼ਬਦ_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    ਜੇ (x86KSnumDrhu == 0) {
        ਵਾਪਸ 0;
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(x86KSnumIOPTLevels > 0);

    ਸ਼ਬਦ_t ਮਾਪ = 1; /* one for the root table */
    ਮਾਪ += N_VTD_CONTEXTS; /* one for each context */
    ਮਾਪ += rmrr_list->ਅੰਕ; /* one for each device */

    ਜੇ (rmrr_list->ਅੰਕ == 0) {
        ਵਾਪਸ ਮਾਪ;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.ਅੰਕ = 1;

    ਲਈ (ਸ਼ਬਦ_t i = 1; i < rmrr_list->ਅੰਕ; i++) {
        ਜੇ (vtd_get_ਜੜ੍ਹ_ਸੂਚਕਾਂਕ(rmrr_list->entries[i].device) !=
            vtd_get_ਜੜ੍ਹ_ਸੂਚਕਾਂਕ(filtered.entries[filtered.ਅੰਕ - 1].device) &&
            rmrr_list->entries[i].ਅਧਾਰ != filtered.entries[filtered.ਅੰਕ - 1].ਅਧਾਰ &&
            rmrr_list->entries[i].ਹੱਦਬੰਦੀ != filtered.entries[filtered.ਅੰਕ - 1].ਹੱਦਬੰਦੀ) {
            filtered.entries[filtered.ਅੰਕ] = rmrr_list->entries[i];
            filtered.ਅੰਕ++;
        }
    }

    ਲਈ (ਸ਼ਬਦ_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        ਜੇ ((VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * i + se_L4_ਪੰਨਾ_ਬਿੱਟ) >= 32) {
            ਮਾਪ++;
        } ਨਹੀਂਤਾਂ {
            ਲਈ (ਸ਼ਬਦ_t j = 0; j < filtered.ਅੰਕ; j++) {
                v_region_t region = (v_region_t) {
                    .ਸ਼ੁਰੂ = filtered.entries[j].ਅਧਾਰ,
                    .ਸਿਰਾ = filtered.entries[j].ਹੱਦਬੰਦੀ
                };
                ਮਾਪ += get_n_paging(region, 32 - (VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * i + se_L4_ਪੰਨਾ_ਬਿੱਟ));
            }
        }
    }
    ਵਾਪਸ ਮਾਪ;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ vtd_map_reserved_ਪੰਨਾ(vtd_cte_t *vtd_context_ਸਾਰਣੀ, ਪੂਰਨ context_ਸੂਚਕਾਂਕ, paddr_t addr)
{
    ਪੂਰਨ i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_ਥਾਂ;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_ਥਾਂ = vtd_context_ਸਾਰਣੀ + context_ਸੂਚਕਾਂਕ;
    ਜੇ (!vtd_cte_ptr_get_present(vtd_context_ਥਾਂ)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_ਸਾਰਣੀ_ਬਿੱਟ);

        *vtd_context_ਥਾਂ = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_ਮੰਜ਼ਿਲ_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_ਥਾਂ, VTD_CTE_ਮਾਪ_ਬਿੱਟ);
    } ਨਹੀਂਤਾਂ {
        iopt = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_cte_ptr_get_asr(vtd_context_ਥਾਂ));
    }
    /* now recursively find and map page tables */
    ਲਈ (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_ਸੂਚਕਾਂਕ;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        ਜੇ (VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * i + se_L4_ਪੰਨਾ_ਬਿੱਟ >= 32) {
            iopt_ਸੂਚਕਾਂਕ = 0;
        } ਨਹੀਂਤਾਂ {
            iopt_ਸੂਚਕਾਂਕ = ((addr >> se_L4_ਪੰਨਾ_ਬਿੱਟ) >> (VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * i)) & ਨਕਾਬ(VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
        }
        vtd_pte_ਥਾਂ = iopt + iopt_ਸੂਚਕਾਂਕ;
        ਜੇ (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_ਥਾਂ = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_ਥਾਂ, VTD_PTE_ਮਾਪ_ਬਿੱਟ);
        } ਨਹੀਂਤਾਂ {
            ਜੇ (!vtd_pte_ptr_get_write(vtd_pte_ਥਾਂ)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_ਸਾਰਣੀ_ਬਿੱਟ);

                *vtd_pte_ਥਾਂ = vtd_pte_new(pptr_ਮੰਜ਼ਿਲ_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_ਥਾਂ, VTD_PTE_ਮਾਪ_ਬਿੱਟ);
            } ਨਹੀਂਤਾਂ {
                iopt = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_pte_ptr_get_addr(vtd_pte_ਥਾਂ));
            }
        }
    }
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ vtd_create_context_ਸਾਰਣੀ(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    ਸ਼ਬਦ_t i;
    vtd_cte_t *vtd_context_ਸਾਰਣੀ = (vtd_cte_t *) it_alloc_paging();

    ਲਿਖੋ("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_ਸਾਰਣੀ);
    flushCacheRange(vtd_context_ਸਾਰਣੀ, VTD_CT_ਮਾਪ_ਬਿੱਟ);

    x86_KSvtd_ਜੜ੍ਹ_ਸਾਰਣੀ[bus] =
        vtd_rte_new(
            pptr_ਮੰਜ਼ਿਲ_paddr(vtd_context_ਸਾਰਣੀ), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    ਲਈ (i = 0; i < rmrr_list->ਅੰਕ; i++) {
        ਜੇ (vtd_get_ਜੜ੍ਹ_ਸੂਚਕਾਂਕ(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            ਲਈ (addr = rmrr_list->entries[i].ਅਧਾਰ; addr < rmrr_list->entries[i].ਹੱਦਬੰਦੀ; addr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ)) {
                vtd_map_reserved_ਪੰਨਾ(vtd_context_ਸਾਰਣੀ, vtd_get_context_ਸੂਚਕਾਂਕ(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_ਕੋਡ ਸਥਿਰ bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t ਸਥਿਤੀ = 0;

    ਲਈ (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)x86_KSvtd_ਜੜ੍ਹ_ਸਾਰਣੀ);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        ਸਥਿਤੀ = vtd_read32(i, GSTS_REG);
        ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, ਸਥਿਤੀ);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        ਜਦੋਂ (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    ਲਈ (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t ਅੰਕੜੇ, addr;

        ਅੰਕੜੇ = int_iommu;
        addr = apic_get_ਅਧਾਰ_paddr();
        ਜੇ (!addr) {
            ਵਾਪਸ false;
        }
        addr |= (cpu_id << 12);

        vtd_process_ਨੁਕਸ(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, ਅੰਕੜੇ);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        ਸਥਿਤੀ = vtd_read32(i, GSTS_REG);
        ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, ਸਥਿਤੀ);
        ਜਦੋਂ (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        ਲਿਖੋ("IOMMU 0x%x: enabling...", i);

        ਸਥਿਤੀ = vtd_read32(i, GSTS_REG);
        ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, ਸਥਿਤੀ);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        ਜਦੋਂ (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        ਲਿਖੋ(" enabled\n");
    }
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ bool_t vtd_init_ਅੰਕ_iopts(uint32_t ਅੰਕ_drhu)
{
    x86KSnumDrhu = ਅੰਕ_drhu;
    x86KSFirstValidIODomain = 0;

    ਜੇ (x86KSnumDrhu == 0) {
        ਵਾਪਸ true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  ਅੰਕ_domain_id_ਬਿੱਟ = 16;
    ਲਈ (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t ਬਿੱਟ_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        ਲਿਖੋ("IOMMU 0x%x: %d-bit domain IDs supported\n", i, ਬਿੱਟ_supported);
        ਜੇ (ਬਿੱਟ_supported < ਅੰਕ_domain_id_ਬਿੱਟ) {
            ਅੰਕ_domain_id_ਬਿੱਟ = ਬਿੱਟ_supported;
        }
    }

    x86KSnumIODomainIDBits = ਅੰਕ_domain_id_ਬਿੱਟ;
    ਅਣਵਰਤਿਆ uint32_t  max_ਅੰਕ_iopt_ਪੱਧਰ;
    ਜੇ (aw_bitmask & SAGAW_6_LEVEL) {
        max_ਅੰਕ_iopt_ਪੱਧਰ = 6;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_5_LEVEL) {
        max_ਅੰਕ_iopt_ਪੱਧਰ = 5;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_4_LEVEL) {
        max_ਅੰਕ_iopt_ਪੱਧਰ = 4;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_3_LEVEL) {
        max_ਅੰਕ_iopt_ਪੱਧਰ = 3;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_2_LEVEL) {
        max_ਅੰਕ_iopt_ਪੱਧਰ = 2;
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        ਵਾਪਸ false;
    }

    ਜੇ (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } ਨਹੀਂਤਾਂ ਜੇ (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        ਵਾਪਸ false;
    }

    ਲਿਖੋ("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_ਅੰਕ_iopt_ਪੱਧਰ);
    ਵਾਪਸ true;
}


BOOT_ਕੋਡ bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    ਜੇ (x86KSnumDrhu == 0) {
        ਵਾਪਸ true;
    }

    x86_KSvtd_ਜੜ੍ਹ_ਸਾਰਣੀ = (vtd_rte_t *) it_alloc_paging();
    ਲਈ (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_ਸਾਰਣੀ(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_ਜੜ੍ਹ_ਸਾਰਣੀ, VTD_RT_ਮਾਪ_ਬਿੱਟ);

    ਜੇ (!vtd_enable(cpu_id)) {
        ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

#ਜੇ_ਅੰਤ /* CONFIG_IOMMU */
