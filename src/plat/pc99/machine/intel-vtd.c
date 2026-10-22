/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU

#ضمن <kernel/boot.h>
#ضمن <machine.h>
#ضمن <machine/io.h>
#ضمن <arch/kernel/apic.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/acpi.h>
#ضمن <plat/machine/intel-vtd.h>
#ضمن <util.h>

#وصف_ڪريو RTADDR_REG  0x20
#وصف_ڪريو GCMD_REG    0x18
#وصف_ڪريو GSTS_REG    0x1C
#وصف_ڪريو CCMD_REG    0x28
#وصف_ڪريو ECAP_REG    0x10
#وصف_ڪريو IOTLB_REG   0x08
#وصف_ڪريو FSTS_REG    0x34
#وصف_ڪريو FECTL_REG   0x38
#وصف_ڪريو FEDATA_REG  0x3C
#وصف_ڪريو FEADDR_REG  0x40
#وصف_ڪريو FEUADDR_REG 0x44
#وصف_ڪريو CAP_REG     0x08

/* Bit Positions within Registers */
#وصف_ڪريو SRTP        30  /* Set Root Table Pointer */
#وصف_ڪريو RTPS        30  /* Root Table Pointer Status */
#وصف_ڪريو TE          31  /* Translation Enable */
#وصف_ڪريو TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#وصف_ڪريو ICC         (31 + 32)  /* Invalidate Context Cache */
#وصف_ڪريو CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#وصف_ڪريو CAIG        27  /* Context Actual Invalidation Granularity */
#وصف_ڪريو CAIG_نقاب   0x3
#وصف_ڪريو IVO_نقاب    0x3FF
#وصف_ڪريو IVT         31  /* Invalidate IOTLB */
#وصف_ڪريو IIRG        28  /* IOTLB Invalidation Request Granularity */
#وصف_ڪريو IAIG        25  /* IOTLB Actual Invalidation Granularity */
#وصف_ڪريو IAIG_نقاب   0x7
#وصف_ڪريو IP          30  /* Interrupt Pending */
#وصف_ڪريو FRI         0x8 /* Fault Recording Index */
#وصف_ڪريو FRI_نقاب    0xFF
#وصف_ڪريو FRO         24
#وصف_ڪريو FRO_نقاب    0xFF
#وصف_ڪريو FI          12
#وصف_ڪريو SID_نقاب    0xFFFF
#وصف_ڪريو SID_BUS(a)  (نقاب(8) & (a >> 8))
#وصف_ڪريو SID_DEV(a)  (نقاب(5) & (a >> 3))
#وصف_ڪريو SID_FUNC(a) (نقاب(3) & a)
#وصف_ڪريو FR_نقاب     0xFF
#وصف_ڪريو FAULT_TYPE  30
#وصف_ڪريو FAULT       31
#وصف_ڪريو NFR         8   /* high word of CAP_REG */
#وصف_ڪريو NFR_نقاب    0xff
#وصف_ڪريو PPF         1
#وصف_ڪريو PPF_نقاب    1
#وصف_ڪريو PRESENT     1
#وصف_ڪريو WBF         27
#وصف_ڪريو WBFS        27
#وصف_ڪريو DID         8
#وصف_ڪريو RW          0x3

#وصف_ڪريو SAGAW         8
#وصف_ڪريو SAGAW_2_LEVEL 0x01
#وصف_ڪريو SAGAW_3_LEVEL 0x02
#وصف_ڪريو SAGAW_4_LEVEL 0x04
#وصف_ڪريو SAGAW_5_LEVEL 0x08
#وصف_ڪريو SAGAW_6_LEVEL 0x10

#وصف_ڪريو CONTEXT_GLOBAL_INVALIDATE 0x1
#وصف_ڪريو IOTLB_GLOBAL_INVALIDATE   0x1

#وصف_ڪريو DMA_TLB_READ_DRAIN  ٻٽو_انگ(17)
#وصف_ڪريو DMA_TLB_WRITE_DRAIN ٻٽو_انگ(16)

#وصف_ڪريو N_VTD_CONTEXTS 256

تعريفنوع uint32_t drhu_id_t;

ساكن مضمن uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t وٿي)
{
    أرجع *(متطاير uint32_t *)(PPTR_DRHU_شروعات + (drhu_id << صفحو_بٽ) + وٿي);
}

ساكن مضمن فراغ vtd_write32(drhu_id_t drhu_id, uint32_t وٿي, uint32_t قدر)
{
    *(متطاير uint32_t *)(PPTR_DRHU_شروعات + (drhu_id << صفحو_بٽ) + وٿي) = قدر;
}


ساكن مضمن uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t وٿي)
{
    أرجع *(متطاير uint64_t *)(PPTR_DRHU_شروعات + (drhu_id << صفحو_بٽ) + وٿي);
}

ساكن مضمن فراغ vtd_write64(drhu_id_t drhu_id, uint32_t وٿي, uint64_t قدر)
{
    *(متطاير uint64_t *)(PPTR_DRHU_شروعات + (drhu_id << صفحو_بٽ) + وٿي) = قدر;
}

ساكن مضمن uint32_t get_ivo(drhu_id_t drhu_id)
{
    أرجع ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_نقاب) * 16;
}

ساكن uint32_t get_fro_وٿي(drhu_id_t drhu_id)
{
    uint32_t fro_وٿي;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_وٿي = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_نقاب;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_وٿي |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    أرجع fro_وٿي << 4;
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
    uint32_t  iotlb_reg_مٿانهون;
    uint32_t  ivo_وٿي;
    drhu_id_t i;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        ivo_وٿي = get_ivo(i);

        /* Wait till IVT bit is clear */
        طالما ((vtd_read32(i, ivo_وٿي + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_مٿانهون = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_مٿانهون |= ٻٽو_انگ(IVT);
        iotlb_reg_مٿانهون |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_وٿي + IOTLB_REG, 0);
        vtd_write32(i, ivo_وٿي + IOTLB_REG + 4, iotlb_reg_مٿانهون);

        /* Wait for the invalidation to complete */
        طالما ((vtd_read32(i, ivo_وٿي + IOTLB_REG + 4) >> IVT) & 1);
    }
}

ساكن فراغ vtd_clear_fault(drhu_id_t i, لفظ_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, ٻٽو_انگ(FAULT));
}

ساكن فراغ vtd_process_نقص(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_وٿي;
    uint32_t ذريعو_id اڻ_استعمال_ٿيل;
    uint32_t fault_type اڻ_استعمال_ٿيل;
    uint32_t پتو[2] اڻ_استعمال_ٿيل;
    uint32_t reason اڻ_استعمال_ٿيل;
    uint32_t انگ_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_حالت;
    uint32_t fault_رڪارڊ_اشاريو;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_وٿي = get_fro_وٿي(i);
    fault_حالت = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_نقاب;

    إذا (fault_حالت) {
        انگ_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_نقاب) + 1;
        fault_رڪارڊ_اشاريو = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_نقاب;
        fr_reg = fro_وٿي + 16 * fault_رڪارڊ_اشاريو;

        /* Traverse the fault register ring buffer */
        افعل {
            ذريعو_id = vtd_read32(i, fr_reg + 8) & SID_نقاب;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            پتو[1] = vtd_read32(i, fr_reg + 4);
            پتو[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_نقاب;

            لکو("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            لکو("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", ذريعو_id,
                   SID_BUS(ذريعو_id), SID_DEV(ذريعو_id), SID_FUNC(ذريعو_id));
            لکو("on address 0x%x:%x ", پتو[1], پتو[0]);
            لکو("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_رڪارڊ_اشاريو = (fault_رڪارڊ_اشاريو + 1) % انگ_fault_regs;
            fr_reg = fro_وٿي + 16 * fault_رڪارڊ_اشاريو;
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
        vtd_process_نقص(i);
    }
}

BOOT_ڪوڊ لفظ_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع 0;
    }
    أكد(x86KSnumIOPTLevels > 0);

    لفظ_t سائيز = 1; /* one for the root table */
    سائيز += N_VTD_CONTEXTS; /* one for each context */
    سائيز += rmrr_list->انگ; /* one for each device */

    إذا (rmrr_list->انگ == 0) {
        أرجع سائيز;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.انگ = 1;

    لكل (لفظ_t i = 1; i < rmrr_list->انگ; i++) {
        إذا (vtd_get_پاڙ_اشاريو(rmrr_list->entries[i].device) !=
            vtd_get_پاڙ_اشاريو(filtered.entries[filtered.انگ - 1].device) &&
            rmrr_list->entries[i].بنياد != filtered.entries[filtered.انگ - 1].بنياد &&
            rmrr_list->entries[i].حد_بندي != filtered.entries[filtered.انگ - 1].حد_بندي) {
            filtered.entries[filtered.انگ] = rmrr_list->entries[i];
            filtered.انگ++;
        }
    }

    لكل (لفظ_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا ((VTD_PT_اشاريو_بٽ * i + se_L4_صفحو_بٽ) >= 32) {
            سائيز++;
        } وإلا {
            لكل (لفظ_t j = 0; j < filtered.انگ; j++) {
                v_region_t region = (v_region_t) {
                    .شروعات = filtered.entries[j].بنياد,
                    .پڇاڙي = filtered.entries[j].حد_بندي
                };
                سائيز += get_n_paging(region, 32 - (VTD_PT_اشاريو_بٽ * i + se_L4_صفحو_بٽ));
            }
        }
    }
    أرجع سائيز;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_ڪوڊ ساكن فراغ vtd_map_reserved_صفحو(vtd_cte_t *vtd_context_جدول, صحيح context_اشاريو, paddr_t addr)
{
    صحيح i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_جڳهه;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_جڳهه = vtd_context_جدول + context_اشاريو;
    إذا (!vtd_cte_ptr_get_present(vtd_context_جڳهه)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_جدول_بٽ);

        *vtd_context_جڳهه = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_منزل_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_جڳهه, VTD_CTE_سائيز_بٽ);
    } وإلا {
        iopt = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_جڳهه));
    }
    /* now recursively find and map page tables */
    لكل (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_اشاريو;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا (VTD_PT_اشاريو_بٽ * i + se_L4_صفحو_بٽ >= 32) {
            iopt_اشاريو = 0;
        } وإلا {
            iopt_اشاريو = ((addr >> se_L4_صفحو_بٽ) >> (VTD_PT_اشاريو_بٽ * i)) & نقاب(VTD_PT_اشاريو_بٽ);
        }
        vtd_pte_جڳهه = iopt + iopt_اشاريو;
        إذا (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_جڳهه = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_جڳهه, VTD_PTE_سائيز_بٽ);
        } وإلا {
            إذا (!vtd_pte_ptr_get_write(vtd_pte_جڳهه)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_جدول_بٽ);

                *vtd_pte_جڳهه = vtd_pte_new(pptr_منزل_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_جڳهه, VTD_PTE_سائيز_بٽ);
            } وإلا {
                iopt = (vtd_pte_t *)paddr_منزل_pptr(vtd_pte_ptr_get_addr(vtd_pte_جڳهه));
            }
        }
    }
}

BOOT_ڪوڊ ساكن فراغ vtd_create_context_جدول(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    لفظ_t i;
    vtd_cte_t *vtd_context_جدول = (vtd_cte_t *) it_alloc_paging();

    لکو("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_جدول);
    flushCacheRange(vtd_context_جدول, VTD_CT_سائيز_بٽ);

    x86_KSvtd_پاڙ_جدول[bus] =
        vtd_rte_new(
            pptr_منزل_paddr(vtd_context_جدول), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    لكل (i = 0; i < rmrr_list->انگ; i++) {
        إذا (vtd_get_پاڙ_اشاريو(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            لكل (addr = rmrr_list->entries[i].بنياد; addr < rmrr_list->entries[i].حد_بندي; addr += ٻٽو_انگ(se_L4_صفحو_بٽ)) {
                vtd_map_reserved_صفحو(vtd_context_جدول, vtd_get_context_اشاريو(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_ڪوڊ ساكن bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t حالت = 0;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_منزل_paddr((فراغ *)x86_KSvtd_پاڙ_جدول);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        حالت = vtd_read32(i, GSTS_REG);
        حالت |= ٻٽو_انگ(SRTP);
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
        addr = apic_get_بنياد_paddr();
        إذا (!addr) {
            أرجع false;
        }
        addr |= (cpu_id << 12);

        vtd_process_نقص(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, معلومات);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        حالت = vtd_read32(i, GSTS_REG);
        حالت |= ٻٽو_انگ(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, حالت);
        طالما (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        لکو("IOMMU 0x%x: enabling...", i);

        حالت = vtd_read32(i, GSTS_REG);
        حالت |= ٻٽو_انگ(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, حالت);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        طالما (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        لکو(" enabled\n");
    }
    أرجع true;
}

BOOT_ڪوڊ bool_t vtd_init_انگ_iopts(uint32_t انگ_drhu)
{
    x86KSnumDrhu = انگ_drhu;
    x86KSFirstValidIODomain = 0;

    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  انگ_domain_id_بٽ = 16;
    لكل (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t بٽ_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        لکو("IOMMU 0x%x: %d-bit domain IDs supported\n", i, بٽ_supported);
        إذا (بٽ_supported < انگ_domain_id_بٽ) {
            انگ_domain_id_بٽ = بٽ_supported;
        }
    }

    x86KSnumIODomainIDBits = انگ_domain_id_بٽ;
    اڻ_استعمال_ٿيل uint32_t  max_انگ_iopt_سطحون;
    إذا (aw_bitmask & SAGAW_6_LEVEL) {
        max_انگ_iopt_سطحون = 6;
    } وإلا إذا (aw_bitmask & SAGAW_5_LEVEL) {
        max_انگ_iopt_سطحون = 5;
    } وإلا إذا (aw_bitmask & SAGAW_4_LEVEL) {
        max_انگ_iopt_سطحون = 4;
    } وإلا إذا (aw_bitmask & SAGAW_3_LEVEL) {
        max_انگ_iopt_سطحون = 3;
    } وإلا إذا (aw_bitmask & SAGAW_2_LEVEL) {
        max_انگ_iopt_سطحون = 2;
    } وإلا {
        لکو("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
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
        لکو("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        أرجع false;
    }

    لکو("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_انگ_iopt_سطحون);
    أرجع true;
}


BOOT_ڪوڊ bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    x86_KSvtd_پاڙ_جدول = (vtd_rte_t *) it_alloc_paging();
    لكل (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_جدول(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_پاڙ_جدول, VTD_RT_سائيز_بٽ);

    إذا (!vtd_enable(cpu_id)) {
        أرجع false;
    }
    أرجع true;
}

#جيڪڏهن_پورو /* CONFIG_IOMMU */
