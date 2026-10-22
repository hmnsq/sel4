/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_IOMMU

#ضمن <kernel/boot.h>
#ضمن <machine.h>
#ضمن <machine/io.h>
#ضمن <arch/kernel/apic.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/acpi.h>
#ضمن <plat/machine/intel-vtd.h>
#ضمن <util.h>

#تعریف_کرو RTADDR_REG  0x20
#تعریف_کرو GCMD_REG    0x18
#تعریف_کرو GSTS_REG    0x1C
#تعریف_کرو CCMD_REG    0x28
#تعریف_کرو ECAP_REG    0x10
#تعریف_کرو IOTLB_REG   0x08
#تعریف_کرو FSTS_REG    0x34
#تعریف_کرو FECTL_REG   0x38
#تعریف_کرو FEDATA_REG  0x3C
#تعریف_کرو FEADDR_REG  0x40
#تعریف_کرو FEUADDR_REG 0x44
#تعریف_کرو CAP_REG     0x08

/* Bit Positions within Registers */
#تعریف_کرو SRTP        30  /* Set Root Table Pointer */
#تعریف_کرو RTPS        30  /* Root Table Pointer Status */
#تعریف_کرو TE          31  /* Translation Enable */
#تعریف_کرو TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#تعریف_کرو ICC         (31 + 32)  /* Invalidate Context Cache */
#تعریف_کرو CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#تعریف_کرو CAIG        27  /* Context Actual Invalidation Granularity */
#تعریف_کرو CAIG_نقاب   0x3
#تعریف_کرو IVO_نقاب    0x3FF
#تعریف_کرو IVT         31  /* Invalidate IOTLB */
#تعریف_کرو IIRG        28  /* IOTLB Invalidation Request Granularity */
#تعریف_کرو IAIG        25  /* IOTLB Actual Invalidation Granularity */
#تعریف_کرو IAIG_نقاب   0x7
#تعریف_کرو IP          30  /* Interrupt Pending */
#تعریف_کرو FRI         0x8 /* Fault Recording Index */
#تعریف_کرو FRI_نقاب    0xFF
#تعریف_کرو FRO         24
#تعریف_کرو FRO_نقاب    0xFF
#تعریف_کرو FI          12
#تعریف_کرو SID_نقاب    0xFFFF
#تعریف_کرو SID_BUS(a)  (نقاب(8) & (a >> 8))
#تعریف_کرو SID_DEV(a)  (نقاب(5) & (a >> 3))
#تعریف_کرو SID_FUNC(a) (نقاب(3) & a)
#تعریف_کرو FR_نقاب     0xFF
#تعریف_کرو FAULT_TYPE  30
#تعریف_کرو FAULT       31
#تعریف_کرو NFR         8   /* high word of CAP_REG */
#تعریف_کرو NFR_نقاب    0xff
#تعریف_کرو PPF         1
#تعریف_کرو PPF_نقاب    1
#تعریف_کرو PRESENT     1
#تعریف_کرو WBF         27
#تعریف_کرو WBFS        27
#تعریف_کرو DID         8
#تعریف_کرو RW          0x3

#تعریف_کرو SAGAW         8
#تعریف_کرو SAGAW_2_LEVEL 0x01
#تعریف_کرو SAGAW_3_LEVEL 0x02
#تعریف_کرو SAGAW_4_LEVEL 0x04
#تعریف_کرو SAGAW_5_LEVEL 0x08
#تعریف_کرو SAGAW_6_LEVEL 0x10

#تعریف_کرو CONTEXT_GLOBAL_INVALIDATE 0x1
#تعریف_کرو IOTLB_GLOBAL_INVALIDATE   0x1

#تعریف_کرو DMA_TLB_READ_DRAIN  ثنائی_ہندسہ(17)
#تعریف_کرو DMA_TLB_WRITE_DRAIN ثنائی_ہندسہ(16)

#تعریف_کرو N_VTD_CONTEXTS 256

تعريفنوع uint32_t drhu_id_t;

ساكن مضمن uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t وتھ)
{
    أرجع *(متطاير uint32_t *)(PPTR_DRHU_شروع + (drhu_id << ورقہ_بٹ) + وتھ);
}

ساكن مضمن فراغ vtd_write32(drhu_id_t drhu_id, uint32_t وتھ, uint32_t قدر)
{
    *(متطاير uint32_t *)(PPTR_DRHU_شروع + (drhu_id << ورقہ_بٹ) + وتھ) = قدر;
}


ساكن مضمن uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t وتھ)
{
    أرجع *(متطاير uint64_t *)(PPTR_DRHU_شروع + (drhu_id << ورقہ_بٹ) + وتھ);
}

ساكن مضمن فراغ vtd_write64(drhu_id_t drhu_id, uint32_t وتھ, uint64_t قدر)
{
    *(متطاير uint64_t *)(PPTR_DRHU_شروع + (drhu_id << ورقہ_بٹ) + وتھ) = قدر;
}

ساكن مضمن uint32_t get_ivo(drhu_id_t drhu_id)
{
    أرجع ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_نقاب) * 16;
}

ساكن uint32_t get_fro_وتھ(drhu_id_t drhu_id)
{
    uint32_t fro_وتھ;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_وتھ = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_نقاب;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_وتھ |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    أرجع fro_وتھ << 4;
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
    uint32_t  iotlb_reg_اتلا;
    uint32_t  ivo_وتھ;
    drhu_id_t i;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        ivo_وتھ = get_ivo(i);

        /* Wait till IVT bit is clear */
        طالما ((vtd_read32(i, ivo_وتھ + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_اتلا = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_اتلا |= ثنائی_ہندسہ(IVT);
        iotlb_reg_اتلا |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_وتھ + IOTLB_REG, 0);
        vtd_write32(i, ivo_وتھ + IOTLB_REG + 4, iotlb_reg_اتلا);

        /* Wait for the invalidation to complete */
        طالما ((vtd_read32(i, ivo_وتھ + IOTLB_REG + 4) >> IVT) & 1);
    }
}

ساكن فراغ vtd_clear_fault(drhu_id_t i, لفظ_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, ثنائی_ہندسہ(FAULT));
}

ساكن فراغ vtd_process_نقص(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_وتھ;
    uint32_t سروت_id بن_ورتیا;
    uint32_t fault_type بن_ورتیا;
    uint32_t پتہ[2] بن_ورتیا;
    uint32_t reason بن_ورتیا;
    uint32_t عدد_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_حالت;
    uint32_t fault_ریکارڈ_اشاریہ;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_وتھ = get_fro_وتھ(i);
    fault_حالت = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_نقاب;

    إذا (fault_حالت) {
        عدد_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_نقاب) + 1;
        fault_ریکارڈ_اشاریہ = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_نقاب;
        fr_reg = fro_وتھ + 16 * fault_ریکارڈ_اشاریہ;

        /* Traverse the fault register ring buffer */
        افعل {
            سروت_id = vtd_read32(i, fr_reg + 8) & SID_نقاب;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            پتہ[1] = vtd_read32(i, fr_reg + 4);
            پتہ[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_نقاب;

            لکھو("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            لکھو("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", سروت_id,
                   SID_BUS(سروت_id), SID_DEV(سروت_id), SID_FUNC(سروت_id));
            لکھو("on address 0x%x:%x ", پتہ[1], پتہ[0]);
            لکھو("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_ریکارڈ_اشاریہ = (fault_ریکارڈ_اشاریہ + 1) % عدد_fault_regs;
            fr_reg = fro_وتھ + 16 * fault_ریکارڈ_اشاریہ;
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

BOOT_کوڈ لفظ_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع 0;
    }
    أكد(x86KSnumIOPTLevels > 0);

    لفظ_t ناپ = 1; /* one for the root table */
    ناپ += N_VTD_CONTEXTS; /* one for each context */
    ناپ += rmrr_list->عدد; /* one for each device */

    إذا (rmrr_list->عدد == 0) {
        أرجع ناپ;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.عدد = 1;

    لكل (لفظ_t i = 1; i < rmrr_list->عدد; i++) {
        إذا (vtd_get_جڑ_اشاریہ(rmrr_list->entries[i].device) !=
            vtd_get_جڑ_اشاریہ(filtered.entries[filtered.عدد - 1].device) &&
            rmrr_list->entries[i].ادھار != filtered.entries[filtered.عدد - 1].ادھار &&
            rmrr_list->entries[i].حد_بندی != filtered.entries[filtered.عدد - 1].حد_بندی) {
            filtered.entries[filtered.عدد] = rmrr_list->entries[i];
            filtered.عدد++;
        }
    }

    لكل (لفظ_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا ((VTD_PT_اشاریہ_بٹ * i + se_L4_ورقہ_بٹ) >= 32) {
            ناپ++;
        } وإلا {
            لكل (لفظ_t j = 0; j < filtered.عدد; j++) {
                v_region_t region = (v_region_t) {
                    .شروع = filtered.entries[j].ادھار,
                    .سرا = filtered.entries[j].حد_بندی
                };
                ناپ += get_n_paging(region, 32 - (VTD_PT_اشاریہ_بٹ * i + se_L4_ورقہ_بٹ));
            }
        }
    }
    أرجع ناپ;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_کوڈ ساكن فراغ vtd_map_reserved_ورقہ(vtd_cte_t *vtd_context_جدول, صحيح context_اشاریہ, paddr_t addr)
{
    صحيح i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_تھاں;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_تھاں = vtd_context_جدول + context_اشاریہ;
    إذا (!vtd_cte_ptr_get_present(vtd_context_تھاں)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_جدول_بٹ);

        *vtd_context_تھاں = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_منزل_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_تھاں, VTD_CTE_ناپ_بٹ);
    } وإلا {
        iopt = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_تھاں));
    }
    /* now recursively find and map page tables */
    لكل (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_اشاریہ;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا (VTD_PT_اشاریہ_بٹ * i + se_L4_ورقہ_بٹ >= 32) {
            iopt_اشاریہ = 0;
        } وإلا {
            iopt_اشاریہ = ((addr >> se_L4_ورقہ_بٹ) >> (VTD_PT_اشاریہ_بٹ * i)) & نقاب(VTD_PT_اشاریہ_بٹ);
        }
        vtd_pte_تھاں = iopt + iopt_اشاریہ;
        إذا (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_تھاں = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_تھاں, VTD_PTE_ناپ_بٹ);
        } وإلا {
            إذا (!vtd_pte_ptr_get_write(vtd_pte_تھاں)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_جدول_بٹ);

                *vtd_pte_تھاں = vtd_pte_new(pptr_منزل_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_تھاں, VTD_PTE_ناپ_بٹ);
            } وإلا {
                iopt = (vtd_pte_t *)paddr_منزل_pptr(vtd_pte_ptr_get_addr(vtd_pte_تھاں));
            }
        }
    }
}

BOOT_کوڈ ساكن فراغ vtd_create_context_جدول(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    لفظ_t i;
    vtd_cte_t *vtd_context_جدول = (vtd_cte_t *) it_alloc_paging();

    لکھو("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_جدول);
    flushCacheRange(vtd_context_جدول, VTD_CT_ناپ_بٹ);

    x86_KSvtd_جڑ_جدول[bus] =
        vtd_rte_new(
            pptr_منزل_paddr(vtd_context_جدول), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    لكل (i = 0; i < rmrr_list->عدد; i++) {
        إذا (vtd_get_جڑ_اشاریہ(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            لكل (addr = rmrr_list->entries[i].ادھار; addr < rmrr_list->entries[i].حد_بندی; addr += ثنائی_ہندسہ(se_L4_ورقہ_بٹ)) {
                vtd_map_reserved_ورقہ(vtd_context_جدول, vtd_get_context_اشاریہ(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_کوڈ ساكن bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t حالت = 0;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_منزل_paddr((فراغ *)x86_KSvtd_جڑ_جدول);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        حالت = vtd_read32(i, GSTS_REG);
        حالت |= ثنائی_ہندسہ(SRTP);
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
        uint32_t انکڑے, addr;

        انکڑے = int_iommu;
        addr = apic_get_ادھار_paddr();
        إذا (!addr) {
            أرجع false;
        }
        addr |= (cpu_id << 12);

        vtd_process_نقص(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, انکڑے);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        حالت = vtd_read32(i, GSTS_REG);
        حالت |= ثنائی_ہندسہ(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, حالت);
        طالما (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        لکھو("IOMMU 0x%x: enabling...", i);

        حالت = vtd_read32(i, GSTS_REG);
        حالت |= ثنائی_ہندسہ(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, حالت);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        طالما (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        لکھو(" enabled\n");
    }
    أرجع true;
}

BOOT_کوڈ bool_t vtd_init_عدد_iopts(uint32_t عدد_drhu)
{
    x86KSnumDrhu = عدد_drhu;
    x86KSFirstValidIODomain = 0;

    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  عدد_domain_id_بٹ = 16;
    لكل (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t بٹ_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        لکھو("IOMMU 0x%x: %d-bit domain IDs supported\n", i, بٹ_supported);
        إذا (بٹ_supported < عدد_domain_id_بٹ) {
            عدد_domain_id_بٹ = بٹ_supported;
        }
    }

    x86KSnumIODomainIDBits = عدد_domain_id_بٹ;
    بن_ورتیا uint32_t  max_عدد_iopt_پدھراں;
    إذا (aw_bitmask & SAGAW_6_LEVEL) {
        max_عدد_iopt_پدھراں = 6;
    } وإلا إذا (aw_bitmask & SAGAW_5_LEVEL) {
        max_عدد_iopt_پدھراں = 5;
    } وإلا إذا (aw_bitmask & SAGAW_4_LEVEL) {
        max_عدد_iopt_پدھراں = 4;
    } وإلا إذا (aw_bitmask & SAGAW_3_LEVEL) {
        max_عدد_iopt_پدھراں = 3;
    } وإلا إذا (aw_bitmask & SAGAW_2_LEVEL) {
        max_عدد_iopt_پدھراں = 2;
    } وإلا {
        لکھو("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
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
        لکھو("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        أرجع false;
    }

    لکھو("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_عدد_iopt_پدھراں);
    أرجع true;
}


BOOT_کوڈ bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    x86_KSvtd_جڑ_جدول = (vtd_rte_t *) it_alloc_paging();
    لكل (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_جدول(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_جڑ_جدول, VTD_RT_ناپ_بٹ);

    إذا (!vtd_enable(cpu_id)) {
        أرجع false;
    }
    أرجع true;
}

#جے_انت /* CONFIG_IOMMU */
