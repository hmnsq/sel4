/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_IOMMU

#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <machine.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/kernel/apic.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/acpi.h>
#అన్తర్భావ <plat/machine/intel-vtd.h>
#అన్తర్భావ <util.h>

#నిర్వచించు RTADDR_REG  0x20
#నిర్వచించు GCMD_REG    0x18
#నిర్వచించు GSTS_REG    0x1C
#నిర్వచించు CCMD_REG    0x28
#నిర్వచించు ECAP_REG    0x10
#నిర్వచించు IOTLB_REG   0x08
#నిర్వచించు FSTS_REG    0x34
#నిర్వచించు FECTL_REG   0x38
#నిర్వచించు FEDATA_REG  0x3C
#నిర్వచించు FEADDR_REG  0x40
#నిర్వచించు FEUADDR_REG 0x44
#నిర్వచించు CAP_REG     0x08

/* Bit Positions within Registers */
#నిర్వచించు SRTP        30  /* Set Root Table Pointer */
#నిర్వచించు RTPS        30  /* Root Table Pointer Status */
#నిర్వచించు TE          31  /* Translation Enable */
#నిర్వచించు TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#నిర్వచించు ICC         (31 + 32)  /* Invalidate Context Cache */
#నిర్వచించు CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#నిర్వచించు CAIG        27  /* Context Actual Invalidation Granularity */
#నిర్వచించు CAIG_ముసుగు   0x3
#నిర్వచించు IVO_ముసుగు    0x3FF
#నిర్వచించు IVT         31  /* Invalidate IOTLB */
#నిర్వచించు IIRG        28  /* IOTLB Invalidation Request Granularity */
#నిర్వచించు IAIG        25  /* IOTLB Actual Invalidation Granularity */
#నిర్వచించు IAIG_ముసుగు   0x7
#నిర్వచించు IP          30  /* Interrupt Pending */
#నిర్వచించు FRI         0x8 /* Fault Recording Index */
#నిర్వచించు FRI_ముసుగు    0xFF
#నిర్వచించు FRO         24
#నిర్వచించు FRO_ముసుగు    0xFF
#నిర్వచించు FI          12
#నిర్వచించు SID_ముసుగు    0xFFFF
#నిర్వచించు SID_BUS(a)  (ముసుగు(8) & (a >> 8))
#నిర్వచించు SID_DEV(a)  (ముసుగు(5) & (a >> 3))
#నిర్వచించు SID_FUNC(a) (ముసుగు(3) & a)
#నిర్వచించు FR_ముసుగు     0xFF
#నిర్వచించు FAULT_TYPE  30
#నిర్వచించు FAULT       31
#నిర్వచించు NFR         8   /* high word of CAP_REG */
#నిర్వచించు NFR_ముసుగు    0xff
#నిర్వచించు PPF         1
#నిర్వచించు PPF_ముసుగు    1
#నిర్వచించు PRESENT     1
#నిర్వచించు WBF         27
#నిర్వచించు WBFS        27
#నిర్వచించు DID         8
#నిర్వచించు RW          0x3

#నిర్వచించు SAGAW         8
#నిర్వచించు SAGAW_2_LEVEL 0x01
#నిర్వచించు SAGAW_3_LEVEL 0x02
#నిర్వచించు SAGAW_4_LEVEL 0x04
#నిర్వచించు SAGAW_5_LEVEL 0x08
#నిర్వచించు SAGAW_6_LEVEL 0x10

#నిర్వచించు CONTEXT_GLOBAL_INVALIDATE 0x1
#నిర్వచించు IOTLB_GLOBAL_INVALIDATE   0x1

#నిర్వచించు DMA_TLB_READ_DRAIN  బిట్టు(17)
#నిర్వచించు DMA_TLB_WRITE_DRAIN బిట్టు(16)

#నిర్వచించు N_VTD_CONTEXTS 256

రకనిర్వచనం uint32_t drhu_id_t;

స్థిర అంతఃవరుస uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t స్థానభ్రంశం)
{
    తిరిగి *(అస్థిర uint32_t *)(PPTR_DRHU_ప్రారంభం + (drhu_id << పుట_బిట్లు) + స్థానభ్రంశం);
}

స్థిర అంతఃవరుస శూన్య vtd_write32(drhu_id_t drhu_id, uint32_t స్థానభ్రంశం, uint32_t విలువ)
{
    *(అస్థిర uint32_t *)(PPTR_DRHU_ప్రారంభం + (drhu_id << పుట_బిట్లు) + స్థానభ్రంశం) = విలువ;
}


స్థిర అంతఃవరుస uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t స్థానభ్రంశం)
{
    తిరిగి *(అస్థిర uint64_t *)(PPTR_DRHU_ప్రారంభం + (drhu_id << పుట_బిట్లు) + స్థానభ్రంశం);
}

స్థిర అంతఃవరుస శూన్య vtd_write64(drhu_id_t drhu_id, uint32_t స్థానభ్రంశం, uint64_t విలువ)
{
    *(అస్థిర uint64_t *)(PPTR_DRHU_ప్రారంభం + (drhu_id << పుట_బిట్లు) + స్థానభ్రంశం) = విలువ;
}

స్థిర అంతఃవరుస uint32_t get_ivo(drhu_id_t drhu_id)
{
    తిరిగి ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_ముసుగు) * 16;
}

స్థిర uint32_t get_fro_స్థానభ్రంశం(drhu_id_t drhu_id)
{
    uint32_t fro_స్థానభ్రంశం;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_స్థానభ్రంశం = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_ముసుగు;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_స్థానభ్రంశం |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    తిరిగి fro_స్థానభ్రంశం << 4;
}

శూన్య invalidate_context_cache(శూన్య)
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

    కొరకు (i = 0; i < x86KSnumDrhu; i++) {
        /* Wait till ICC bit is clear */
        uint64_t ccmd = 0;
        అయితే ((vtd_read64(i, CCMD_REG) >> ICC) & 1);

        /* Program CIRG for Global Invalidation by setting bit 61 which
         * will be bit 29 in upper 32 bits of CCMD_REG
         */
        ccmd = ((uint64_t)CONTEXT_GLOBAL_INVALIDATE << CIRG) | (1ull << ICC);

        /* Invalidate Context Cache */
        vtd_write64(i, CCMD_REG, ccmd);

        /* Wait for the invalidation to complete */
        అయితే ((vtd_read64(i, CCMD_REG) >> ICC) & 1);
    }
}

శూన్య invalidate_iotlb(శూన్య)
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
    uint32_t  iotlb_reg_పై;
    uint32_t  ivo_స్థానభ్రంశం;
    drhu_id_t i;

    కొరకు (i = 0; i < x86KSnumDrhu; i++) {
        ivo_స్థానభ్రంశం = get_ivo(i);

        /* Wait till IVT bit is clear */
        అయితే ((vtd_read32(i, ivo_స్థానభ్రంశం + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_పై = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_పై |= బిట్టు(IVT);
        iotlb_reg_పై |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_స్థానభ్రంశం + IOTLB_REG, 0);
        vtd_write32(i, ivo_స్థానభ్రంశం + IOTLB_REG + 4, iotlb_reg_పై);

        /* Wait for the invalidation to complete */
        అయితే ((vtd_read32(i, ivo_స్థానభ్రంశం + IOTLB_REG + 4) >> IVT) & 1);
    }
}

స్థిర శూన్య vtd_clear_fault(drhu_id_t i, మాట_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, బిట్టు(FAULT));
}

స్థిర శూన్య vtd_process_దోషాలు(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_స్థానభ్రంశం;
    uint32_t ఆధారం_id వాడని;
    uint32_t fault_type వాడని;
    uint32_t చిరునామా[2] వాడని;
    uint32_t reason వాడని;
    uint32_t అంకె_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_స్థితి;
    uint32_t fault_రికార్డు_సూచీ;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_స్థానభ్రంశం = get_fro_స్థానభ్రంశం(i);
    fault_స్థితి = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_ముసుగు;

    ఒకవేళ (fault_స్థితి) {
        అంకె_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_ముసుగు) + 1;
        fault_రికార్డు_సూచీ = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_ముసుగు;
        fr_reg = fro_స్థానభ్రంశం + 16 * fault_రికార్డు_సూచీ;

        /* Traverse the fault register ring buffer */
        చేయి {
            ఆధారం_id = vtd_read32(i, fr_reg + 8) & SID_ముసుగు;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            చిరునామా[1] = vtd_read32(i, fr_reg + 4);
            చిరునామా[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_ముసుగు;

            రాయి("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            రాయి("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", ఆధారం_id,
                   SID_BUS(ఆధారం_id), SID_DEV(ఆధారం_id), SID_FUNC(ఆధారం_id));
            రాయి("on address 0x%x:%x ", చిరునామా[1], చిరునామా[0]);
            రాయి("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_రికార్డు_సూచీ = (fault_రికార్డు_సూచీ + 1) % అంకె_fault_regs;
            fr_reg = fro_స్థానభ్రంశం + 16 * fault_రికార్డు_సూచీ;
        } అయితే ((vtd_read32(i, fr_reg + 12) >> FAULT) & 1);

        /* Check for Primary Fault Overflow */
        ఒకవేళ (vtd_read32(i, FSTS_REG) & 1) {
            /* Clear PFO bit, so new faults will be generated again ! */
            vtd_write32(i, FSTS_REG, 1);
        }
    }
}

శూన్య vtd_handle_fault(శూన్య)
{
    drhu_id_t i;

    కొరకు (i = 0; i < x86KSnumDrhu; i++) {
        vtd_process_దోషాలు(i);
    }
}

BOOT_సంకేతం మాట_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    ఒకవేళ (x86KSnumDrhu == 0) {
        తిరిగి 0;
    }
    ప్రతిజ్ఞా(x86KSnumIOPTLevels > 0);

    మాట_t కొలత = 1; /* one for the root table */
    కొలత += N_VTD_CONTEXTS; /* one for each context */
    కొలత += rmrr_list->అంకె; /* one for each device */

    ఒకవేళ (rmrr_list->అంకె == 0) {
        తిరిగి కొలత;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.అంకె = 1;

    కొరకు (మాట_t i = 1; i < rmrr_list->అంకె; i++) {
        ఒకవేళ (vtd_get_మూలం_సూచీ(rmrr_list->entries[i].device) !=
            vtd_get_మూలం_సూచీ(filtered.entries[filtered.అంకె - 1].device) &&
            rmrr_list->entries[i].పునాది != filtered.entries[filtered.అంకె - 1].పునాది &&
            rmrr_list->entries[i].హద్దు != filtered.entries[filtered.అంకె - 1].హద్దు) {
            filtered.entries[filtered.అంకె] = rmrr_list->entries[i];
            filtered.అంకె++;
        }
    }

    కొరకు (మాట_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        ఒకవేళ ((VTD_PT_సూచీ_బిట్లు * i + se_L4_పుట_బిట్లు) >= 32) {
            కొలత++;
        } లేకపోతే {
            కొరకు (మాట_t j = 0; j < filtered.అంకె; j++) {
                v_region_t region = (v_region_t) {
                    .ప్రారంభం = filtered.entries[j].పునాది,
                    .చివర = filtered.entries[j].హద్దు
                };
                కొలత += get_n_paging(region, 32 - (VTD_PT_సూచీ_బిట్లు * i + se_L4_పుట_బిట్లు));
            }
        }
    }
    తిరిగి కొలత;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_సంకేతం స్థిర శూన్య vtd_map_reserved_పుట(vtd_cte_t *vtd_context_పట్టిక, పూర్ణాంకం context_సూచీ, paddr_t addr)
{
    పూర్ణాంకం i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_స్థానం;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_స్థానం = vtd_context_పట్టిక + context_సూచీ;
    ఒకవేళ (!vtd_cte_ptr_get_present(vtd_context_స్థానం)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_పట్టిక_బిట్లు);

        *vtd_context_స్థానం = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_గమ్యం_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_స్థానం, VTD_CTE_కొలత_బిట్లు);
    } లేకపోతే {
        iopt = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_cte_ptr_get_asr(vtd_context_స్థానం));
    }
    /* now recursively find and map page tables */
    కొరకు (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_సూచీ;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        ఒకవేళ (VTD_PT_సూచీ_బిట్లు * i + se_L4_పుట_బిట్లు >= 32) {
            iopt_సూచీ = 0;
        } లేకపోతే {
            iopt_సూచీ = ((addr >> se_L4_పుట_బిట్లు) >> (VTD_PT_సూచీ_బిట్లు * i)) & ముసుగు(VTD_PT_సూచీ_బిట్లు);
        }
        vtd_pte_స్థానం = iopt + iopt_సూచీ;
        ఒకవేళ (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_స్థానం = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_స్థానం, VTD_PTE_కొలత_బిట్లు);
        } లేకపోతే {
            ఒకవేళ (!vtd_pte_ptr_get_write(vtd_pte_స్థానం)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_పట్టిక_బిట్లు);

                *vtd_pte_స్థానం = vtd_pte_new(pptr_గమ్యం_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_స్థానం, VTD_PTE_కొలత_బిట్లు);
            } లేకపోతే {
                iopt = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_pte_ptr_get_addr(vtd_pte_స్థానం));
            }
        }
    }
}

BOOT_సంకేతం స్థిర శూన్య vtd_create_context_పట్టిక(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    మాట_t i;
    vtd_cte_t *vtd_context_పట్టిక = (vtd_cte_t *) it_alloc_paging();

    రాయి("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_పట్టిక);
    flushCacheRange(vtd_context_పట్టిక, VTD_CT_కొలత_బిట్లు);

    x86_KSvtd_మూలం_పట్టిక[bus] =
        vtd_rte_new(
            pptr_గమ్యం_paddr(vtd_context_పట్టిక), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    కొరకు (i = 0; i < rmrr_list->అంకె; i++) {
        ఒకవేళ (vtd_get_మూలం_సూచీ(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            కొరకు (addr = rmrr_list->entries[i].పునాది; addr < rmrr_list->entries[i].హద్దు; addr += బిట్టు(se_L4_పుట_బిట్లు)) {
                vtd_map_reserved_పుట(vtd_context_పట్టిక, vtd_get_context_సూచీ(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_సంకేతం స్థిర bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t స్థితి_2 = 0;

    కొరకు (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_గమ్యం_paddr((శూన్య *)x86_KSvtd_మూలం_పట్టిక);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        స్థితి_2 = vtd_read32(i, GSTS_REG);
        స్థితి_2 |= బిట్టు(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, స్థితి_2);

        /* Wait for SRTP operation to complete by polling
         * RTPS bit from GSTS_REG
         */
        అయితే (!((vtd_read32(i, GSTS_REG) >> RTPS) & 1));
    }

    /* Globally invalidate context cache of all IOMMUs */
    invalidate_context_cache();

    /* Globally invalidate IOTLB of all IOMMUs */
    invalidate_iotlb();

    కొరకు (i = 0; i < x86KSnumDrhu; i++) {
        uint32_t దత్తాంశం, addr;

        దత్తాంశం = int_iommu;
        addr = apic_get_పునాది_paddr();
        ఒకవేళ (!addr) {
            తిరిగి false;
        }
        addr |= (cpu_id << 12);

        vtd_process_దోషాలు(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, దత్తాంశం);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        స్థితి_2 = vtd_read32(i, GSTS_REG);
        స్థితి_2 |= బిట్టు(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, స్థితి_2);
        అయితే (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        రాయి("IOMMU 0x%x: enabling...", i);

        స్థితి_2 = vtd_read32(i, GSTS_REG);
        స్థితి_2 |= బిట్టు(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, స్థితి_2);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        అయితే (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        రాయి(" enabled\n");
    }
    తిరిగి true;
}

BOOT_సంకేతం bool_t vtd_init_అంకె_iopts(uint32_t అంకె_drhu)
{
    x86KSnumDrhu = అంకె_drhu;
    x86KSFirstValidIODomain = 0;

    ఒకవేళ (x86KSnumDrhu == 0) {
        తిరిగి true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  అంకె_domain_id_బిట్లు = 16;
    కొరకు (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t బిట్లు_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        రాయి("IOMMU 0x%x: %d-bit domain IDs supported\n", i, బిట్లు_supported);
        ఒకవేళ (బిట్లు_supported < అంకె_domain_id_బిట్లు) {
            అంకె_domain_id_బిట్లు = బిట్లు_supported;
        }
    }

    x86KSnumIODomainIDBits = అంకె_domain_id_బిట్లు;
    వాడని uint32_t  max_అంకె_iopt_స్థాయిలు;
    ఒకవేళ (aw_bitmask & SAGAW_6_LEVEL) {
        max_అంకె_iopt_స్థాయిలు = 6;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_5_LEVEL) {
        max_అంకె_iopt_స్థాయిలు = 5;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_4_LEVEL) {
        max_అంకె_iopt_స్థాయిలు = 4;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_3_LEVEL) {
        max_అంకె_iopt_స్థాయిలు = 3;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_2_LEVEL) {
        max_అంకె_iopt_స్థాయిలు = 2;
    } లేకపోతే {
        రాయి("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        తిరిగి false;
    }

    ఒకవేళ (aw_bitmask & SAGAW_3_LEVEL) {
        x86KSnumIOPTLevels = 3;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_4_LEVEL) {
        x86KSnumIOPTLevels = 4;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_5_LEVEL) {
        x86KSnumIOPTLevels = 5;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_6_LEVEL) {
        x86KSnumIOPTLevels = 6;
    } లేకపోతే ఒకవేళ (aw_bitmask & SAGAW_2_LEVEL) {
        x86KSnumIOPTLevels = 2;
    } లేకపోతే {
        రాయి("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        తిరిగి false;
    }

    రాయి("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_అంకె_iopt_స్థాయిలు);
    తిరిగి true;
}


BOOT_సంకేతం bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    ఒకవేళ (x86KSnumDrhu == 0) {
        తిరిగి true;
    }

    x86_KSvtd_మూలం_పట్టిక = (vtd_rte_t *) it_alloc_paging();
    కొరకు (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_పట్టిక(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_మూలం_పట్టిక, VTD_RT_కొలత_బిట్లు);

    ఒకవేళ (!vtd_enable(cpu_id)) {
        తిరిగి false;
    }
    తిరిగి true;
}

#అయితే_ముగింపు /* CONFIG_IOMMU */
