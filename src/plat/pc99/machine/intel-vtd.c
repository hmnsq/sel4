/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_IOMMU

#ضمن <kernel/boot.h>
#ضمن <machine.h>
#ضمن <machine/io.h>
#ضمن <arch/kernel/apic.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/acpi.h>
#ضمن <plat/machine/intel-vtd.h>
#ضمن <util.h>

#تعریف_کن RTADDR_REG  0x20
#تعریف_کن GCMD_REG    0x18
#تعریف_کن GSTS_REG    0x1C
#تعریف_کن CCMD_REG    0x28
#تعریف_کن ECAP_REG    0x10
#تعریف_کن IOTLB_REG   0x08
#تعریف_کن FSTS_REG    0x34
#تعریف_کن FECTL_REG   0x38
#تعریف_کن FEDATA_REG  0x3C
#تعریف_کن FEADDR_REG  0x40
#تعریف_کن FEUADDR_REG 0x44
#تعریف_کن CAP_REG     0x08

/* Bit Positions within Registers */
#تعریف_کن SRTP        30  /* Set Root Table Pointer */
#تعریف_کن RTPS        30  /* Root Table Pointer Status */
#تعریف_کن TE          31  /* Translation Enable */
#تعریف_کن TES         31  /* Translation Enable Status */

/* ICC is 63rd bit in CCMD_REG, but since we will be
 * accessing this register as 4 byte word, ICC becomes
 * 31st bit in the upper 32bit word.
 */
#تعریف_کن ICC         (31 + 32)  /* Invalidate Context Cache */
#تعریف_کن CIRG        (29 + 32) /* Context Invalidation Request Granularity */
#تعریف_کن CAIG        27  /* Context Actual Invalidation Granularity */
#تعریف_کن CAIG_ماسک   0x3
#تعریف_کن IVO_ماسک    0x3FF
#تعریف_کن IVT         31  /* Invalidate IOTLB */
#تعریف_کن IIRG        28  /* IOTLB Invalidation Request Granularity */
#تعریف_کن IAIG        25  /* IOTLB Actual Invalidation Granularity */
#تعریف_کن IAIG_ماسک   0x7
#تعریف_کن IP          30  /* Interrupt Pending */
#تعریف_کن FRI         0x8 /* Fault Recording Index */
#تعریف_کن FRI_ماسک    0xFF
#تعریف_کن FRO         24
#تعریف_کن FRO_ماسک    0xFF
#تعریف_کن FI          12
#تعریف_کن SID_ماسک    0xFFFF
#تعریف_کن SID_BUS(a)  (ماسک(8) & (a >> 8))
#تعریف_کن SID_DEV(a)  (ماسک(5) & (a >> 3))
#تعریف_کن SID_FUNC(a) (ماسک(3) & a)
#تعریف_کن FR_ماسک     0xFF
#تعریف_کن FAULT_TYPE  30
#تعریف_کن FAULT       31
#تعریف_کن NFR         8   /* high word of CAP_REG */
#تعریف_کن NFR_ماسک    0xff
#تعریف_کن PPF         1
#تعریف_کن PPF_ماسک    1
#تعریف_کن PRESENT     1
#تعریف_کن WBF         27
#تعریف_کن WBFS        27
#تعریف_کن DID         8
#تعریف_کن RW          0x3

#تعریف_کن SAGAW         8
#تعریف_کن SAGAW_2_LEVEL 0x01
#تعریف_کن SAGAW_3_LEVEL 0x02
#تعریف_کن SAGAW_4_LEVEL 0x04
#تعریف_کن SAGAW_5_LEVEL 0x08
#تعریف_کن SAGAW_6_LEVEL 0x10

#تعریف_کن CONTEXT_GLOBAL_INVALIDATE 0x1
#تعریف_کن IOTLB_GLOBAL_INVALIDATE   0x1

#تعریف_کن DMA_TLB_READ_DRAIN  بیت(17)
#تعریف_کن DMA_TLB_WRITE_DRAIN بیت(16)

#تعریف_کن N_VTD_CONTEXTS 256

تعريفنوع uint32_t drhu_id_t;

ساكن مضمن uint32_t vtd_read32(drhu_id_t drhu_id, uint32_t جابه_جایی)
{
    أرجع *(متطاير uint32_t *)(PPTR_DRHU_شروع + (drhu_id << صفحه_بیتها) + جابه_جایی);
}

ساكن مضمن فراغ vtd_write32(drhu_id_t drhu_id, uint32_t جابه_جایی, uint32_t مقدار)
{
    *(متطاير uint32_t *)(PPTR_DRHU_شروع + (drhu_id << صفحه_بیتها) + جابه_جایی) = مقدار;
}


ساكن مضمن uint64_t vtd_read64(drhu_id_t drhu_id, uint32_t جابه_جایی)
{
    أرجع *(متطاير uint64_t *)(PPTR_DRHU_شروع + (drhu_id << صفحه_بیتها) + جابه_جایی);
}

ساكن مضمن فراغ vtd_write64(drhu_id_t drhu_id, uint32_t جابه_جایی, uint64_t مقدار)
{
    *(متطاير uint64_t *)(PPTR_DRHU_شروع + (drhu_id << صفحه_بیتها) + جابه_جایی) = مقدار;
}

ساكن مضمن uint32_t get_ivo(drhu_id_t drhu_id)
{
    أرجع ((vtd_read32(drhu_id, ECAP_REG) >> 8) & IVO_ماسک) * 16;
}

ساكن uint32_t get_fro_جابه_جایی(drhu_id_t drhu_id)
{
    uint32_t fro_جابه_جایی;

    /* Get bits 31 to 24 from lower Capability Register */
    fro_جابه_جایی = (vtd_read32(drhu_id, CAP_REG) >> FRO) & FRO_ماسک;

    /* Get bits 33 to 32 from higher Capability Register */
    fro_جابه_جایی |= (vtd_read32(drhu_id, CAP_REG + 4) & 0x3) << 8;

    أرجع fro_جابه_جایی << 4;
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
    uint32_t  iotlb_reg_فوقانی;
    uint32_t  ivo_جابه_جایی;
    drhu_id_t i;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        ivo_جابه_جایی = get_ivo(i);

        /* Wait till IVT bit is clear */
        طالما ((vtd_read32(i, ivo_جابه_جایی + IOTLB_REG + 4) >> IVT) & 1);

        /* Program IIRG for Global Invalidation by setting bit 60 which
         * will be bit 28 in upper 32 bits of IOTLB_REG
         */
        iotlb_reg_فوقانی = invalidate_command << IIRG;

        /* Invalidate IOTLB */
        iotlb_reg_فوقانی |= بیت(IVT);
        iotlb_reg_فوقانی |= DMA_TLB_READ_DRAIN | DMA_TLB_WRITE_DRAIN;

        vtd_write32(i, ivo_جابه_جایی + IOTLB_REG, 0);
        vtd_write32(i, ivo_جابه_جایی + IOTLB_REG + 4, iotlb_reg_فوقانی);

        /* Wait for the invalidation to complete */
        طالما ((vtd_read32(i, ivo_جابه_جایی + IOTLB_REG + 4) >> IVT) & 1);
    }
}

ساكن فراغ vtd_clear_fault(drhu_id_t i, کلمه_t fr_reg)
{
    /* Clear the 'F' (Fault) bit to indicate that this fault is processed */
    vtd_write32(i, fr_reg + 12, بیت(FAULT));
}

ساكن فراغ vtd_process_خطاها(drhu_id_t i)
{
    /* Fault Recording register offset relative to the base register */
    uint32_t fro_جابه_جایی;
    uint32_t منبع_id استفادهنشده;
    uint32_t fault_type استفادهنشده;
    uint32_t آدرس[2] استفادهنشده;
    uint32_t reason استفادهنشده;
    uint32_t رقم_fault_regs;
    uint32_t fr_reg;
    uint32_t fault_وضعیت;
    uint32_t fault_رکورد_شاخص;

    /* Retrieves FRO by looking into Capability register bits 33 to 24 */
    fro_جابه_جایی = get_fro_جابه_جایی(i);
    fault_وضعیت = (vtd_read32(i, FSTS_REG) >> PPF) & PPF_ماسک;

    إذا (fault_وضعیت) {
        رقم_fault_regs = ((vtd_read32(i, CAP_REG + 4) >> NFR) & NFR_ماسک) + 1;
        fault_رکورد_شاخص = (vtd_read32(i, FSTS_REG) >> FRI) & FRI_ماسک;
        fr_reg = fro_جابه_جایی + 16 * fault_رکورد_شاخص;

        /* Traverse the fault register ring buffer */
        افعل {
            منبع_id = vtd_read32(i, fr_reg + 8) & SID_ماسک;

            fault_type = (vtd_read32(i, fr_reg + 12) >> FAULT_TYPE) & 1;
            آدرس[1] = vtd_read32(i, fr_reg + 4);
            آدرس[0] = vtd_read32(i, fr_reg);
            reason = vtd_read32(i, fr_reg + 12) & FR_ماسک;

            چاپ_کن("IOMMU: DMA %s page fault ", fault_type ? "read" : "write");
            چاپ_کن("from 0x%x (bus: 0x%lx/dev: 0x%lx/fun: 0x%lx) ", منبع_id,
                   SID_BUS(منبع_id), SID_DEV(منبع_id), SID_FUNC(منبع_id));
            چاپ_کن("on address 0x%x:%x ", آدرس[1], آدرس[0]);
            چاپ_کن("with reason code 0x%x\n", reason);

            vtd_clear_fault(i, fr_reg);

            fault_رکورد_شاخص = (fault_رکورد_شاخص + 1) % رقم_fault_regs;
            fr_reg = fro_جابه_جایی + 16 * fault_رکورد_شاخص;
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
        vtd_process_خطاها(i);
    }
}

BOOT_کد کلمه_t vtd_get_n_paging(acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع 0;
    }
    أكد(x86KSnumIOPTLevels > 0);

    کلمه_t اندازه = 1; /* one for the root table */
    اندازه += N_VTD_CONTEXTS; /* one for each context */
    اندازه += rmrr_list->رقم; /* one for each device */

    إذا (rmrr_list->رقم == 0) {
        أرجع اندازه;
    }

    /* filter the identical regions by pci bus id */
    acpi_rmrr_list_t filtered;
    filtered.entries[0] = rmrr_list->entries[0];
    filtered.رقم = 1;

    لكل (کلمه_t i = 1; i < rmrr_list->رقم; i++) {
        إذا (vtd_get_ریشه_شاخص(rmrr_list->entries[i].device) !=
            vtd_get_ریشه_شاخص(filtered.entries[filtered.رقم - 1].device) &&
            rmrr_list->entries[i].پایه != filtered.entries[filtered.رقم - 1].پایه &&
            rmrr_list->entries[i].حد != filtered.entries[filtered.رقم - 1].حد) {
            filtered.entries[filtered.رقم] = rmrr_list->entries[i];
            filtered.رقم++;
        }
    }

    لكل (کلمه_t i = x86KSnumIOPTLevels - 1; i > 0; i--) {
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا ((VTD_PT_شاخص_بیتها * i + se_L4_صفحه_بیتها) >= 32) {
            اندازه++;
        } وإلا {
            لكل (کلمه_t j = 0; j < filtered.رقم; j++) {
                v_region_t region = (v_region_t) {
                    .شروع = filtered.entries[j].پایه,
                    .انتها = filtered.entries[j].حد
                };
                اندازه += get_n_paging(region, 32 - (VTD_PT_شاخص_بیتها * i + se_L4_صفحه_بیتها));
            }
        }
    }
    أرجع اندازه;
}

/* This function is a simplistic duplication of some of the logic
 * in iospace.c
 */
BOOT_کد ساكن فراغ vtd_map_reserved_صفحه(vtd_cte_t *vtd_context_جدول, صحيح context_شاخص, paddr_t addr)
{
    صحيح i;
    vtd_pte_t *iopt;
    vtd_pte_t *vtd_pte_جایگاه;
    /* first check for the first page table */
    vtd_cte_t *vtd_context_جایگاه = vtd_context_جدول + context_شاخص;
    إذا (!vtd_cte_ptr_get_present(vtd_context_جایگاه)) {
        iopt = (vtd_pte_t *) it_alloc_paging();
        flushCacheRange(iopt, se_L4_IOPage_جدول_بیتها);

        *vtd_context_جایگاه = vtd_cte_new(
                                x86KSFirstValidIODomain,  /* Domain ID                              */
                                true,                     /* RMRR Mapping                           */
                                x86KSnumIOPTLevels - 2,   /* Address Width                          */
                                pptr_مقصد_paddr(iopt),      /* Address Space Root                     */
                                0,                        /* Translation Type                       */
                                true);                    /* Present                                */
        x86KSFirstValidIODomain++;
        flushCacheRange(vtd_context_جایگاه, VTD_CTE_اندازه_بیتها);
    } وإلا {
        iopt = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_جایگاه));
    }
    /* now recursively find and map page tables */
    لكل (i = x86KSnumIOPTLevels - 1; i >= 0; i--) {
        uint32_t iopt_شاخص;
        /* If we are still looking up bits beyond the 32bit of physical
         * that we support then we select entry 0 in the current PT */
        إذا (VTD_PT_شاخص_بیتها * i + se_L4_صفحه_بیتها >= 32) {
            iopt_شاخص = 0;
        } وإلا {
            iopt_شاخص = ((addr >> se_L4_صفحه_بیتها) >> (VTD_PT_شاخص_بیتها * i)) & ماسک(VTD_PT_شاخص_بیتها);
        }
        vtd_pte_جایگاه = iopt + iopt_شاخص;
        إذا (i == 0) {
            /* Now put the mapping in */
            *vtd_pte_جایگاه = vtd_pte_new(addr, 1, 1);
            flushCacheRange(vtd_pte_جایگاه, VTD_PTE_اندازه_بیتها);
        } وإلا {
            إذا (!vtd_pte_ptr_get_write(vtd_pte_جایگاه)) {
                iopt = (vtd_pte_t *) it_alloc_paging();
                flushCacheRange(iopt, se_L4_IOPage_جدول_بیتها);

                *vtd_pte_جایگاه = vtd_pte_new(pptr_مقصد_paddr(iopt), 1, 1);
                flushCacheRange(vtd_pte_جایگاه, VTD_PTE_اندازه_بیتها);
            } وإلا {
                iopt = (vtd_pte_t *)paddr_مقصد_pptr(vtd_pte_ptr_get_addr(vtd_pte_جایگاه));
            }
        }
    }
}

BOOT_کد ساكن فراغ vtd_create_context_جدول(uint8_t bus, acpi_rmrr_list_t *rmrr_list)
{
    کلمه_t i;
    vtd_cte_t *vtd_context_جدول = (vtd_cte_t *) it_alloc_paging();

    چاپ_کن("IOMMU: Create VTD context table for PCI bus 0x%x (pptr=%p)\n", bus, vtd_context_جدول);
    flushCacheRange(vtd_context_جدول, VTD_CT_اندازه_بیتها);

    x86_KSvtd_ریشه_جدول[bus] =
        vtd_rte_new(
            pptr_مقصد_paddr(vtd_context_جدول), /* Context Table Pointer */
            true                                           /* Present               */
        );
    /* map in any RMRR regions */
    لكل (i = 0; i < rmrr_list->رقم; i++) {
        إذا (vtd_get_ریشه_شاخص(rmrr_list->entries[i].device) == bus) {
            uint32_t addr;
            لكل (addr = rmrr_list->entries[i].پایه; addr < rmrr_list->entries[i].حد; addr += بیت(se_L4_صفحه_بیتها)) {
                vtd_map_reserved_صفحه(vtd_context_جدول, vtd_get_context_شاخص(rmrr_list->entries[i].device), addr);
            }
        }
    }
}

BOOT_کد ساكن bool_t vtd_enable(cpu_id_t cpu_id)
{
    drhu_id_t i;
    uint32_t وضعیت = 0;

    لكل (i = 0; i < x86KSnumDrhu; i++) {
        pptr_t pa = (pptr_t)pptr_مقصد_paddr((فراغ *)x86_KSvtd_ریشه_جدول);

        /* Set the Root Table Register */
        vtd_write64(i, RTADDR_REG, pa);
        وضعیت = vtd_read32(i, GSTS_REG);
        وضعیت |= بیت(SRTP);
        /* Set SRTP bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, وضعیت);

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
        uint32_t داده, addr;

        داده = int_iommu;
        addr = apic_get_پایه_paddr();
        إذا (!addr) {
            أرجع false;
        }
        addr |= (cpu_id << 12);

        vtd_process_خطاها(i);
        vtd_write32(i, FECTL_REG, 0);
        vtd_write32(i, FEDATA_REG, داده);
        vtd_write32(i, FEADDR_REG, addr);
        vtd_write32(i, FEUADDR_REG, 0);
        وضعیت = vtd_read32(i, GSTS_REG);
        وضعیت |= بیت(WBF);
        /*flush IOMMU write buffer */
        vtd_write32(i, GCMD_REG, وضعیت);
        طالما (((vtd_read32(i, GSTS_REG) >> WBFS) & 1));

        چاپ_کن("IOMMU 0x%x: enabling...", i);

        وضعیت = vtd_read32(i, GSTS_REG);
        وضعیت |= بیت(TE);
        /* Enable the DMA translation by setting TE bit in GCMD_REG */
        vtd_write32(i, GCMD_REG, وضعیت);

        /* Wait for Translation Enable operation to complete by polling
         * TES bit from GSTS_REG
         */
        طالما (!((vtd_read32(i, GSTS_REG) >> TES) & 1));

        چاپ_کن(" enabled\n");
    }
    أرجع true;
}

BOOT_کد bool_t vtd_init_رقم_iopts(uint32_t رقم_drhu)
{
    x86KSnumDrhu = رقم_drhu;
    x86KSFirstValidIODomain = 0;

    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    uint32_t aw_bitmask = 0xffffffff;
    /* Start the number of domains at 16 bits */
    uint32_t  رقم_domain_id_بیتها = 16;
    لكل (drhu_id_t i = 0; i < x86KSnumDrhu; i++) {
        uint32_t بیتها_supported = 4 + 2 * (vtd_read32(i, CAP_REG) & 7);
        aw_bitmask &= vtd_read32(i, CAP_REG) >> SAGAW;
        چاپ_کن("IOMMU 0x%x: %d-bit domain IDs supported\n", i, بیتها_supported);
        إذا (بیتها_supported < رقم_domain_id_بیتها) {
            رقم_domain_id_بیتها = بیتها_supported;
        }
    }

    x86KSnumIODomainIDBits = رقم_domain_id_بیتها;
    استفادهنشده uint32_t  max_رقم_iopt_سطوح;
    إذا (aw_bitmask & SAGAW_6_LEVEL) {
        max_رقم_iopt_سطوح = 6;
    } وإلا إذا (aw_bitmask & SAGAW_5_LEVEL) {
        max_رقم_iopt_سطوح = 5;
    } وإلا إذا (aw_bitmask & SAGAW_4_LEVEL) {
        max_رقم_iopt_سطوح = 4;
    } وإلا إذا (aw_bitmask & SAGAW_3_LEVEL) {
        max_رقم_iopt_سطوح = 3;
    } وإلا إذا (aw_bitmask & SAGAW_2_LEVEL) {
        max_رقم_iopt_سطوح = 2;
    } وإلا {
        چاپ_کن("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
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
        چاپ_کن("IOMMU: mismatch of supported number of PT levels between IOMMUs\n");
        أرجع false;
    }

    چاپ_کن("IOMMU: Using %d page-table levels (max. supported: %d)\n", x86KSnumIOPTLevels, max_رقم_iopt_سطوح);
    أرجع true;
}


BOOT_کد bool_t vtd_init(cpu_id_t  cpu_id, acpi_rmrr_list_t *rmrr_list)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع true;
    }

    x86_KSvtd_ریشه_جدول = (vtd_rte_t *) it_alloc_paging();
    لكل (uint32_t bus = 0; bus < N_VTD_CONTEXTS; bus++) {
        vtd_create_context_جدول(bus, rmrr_list);
    }

    flushCacheRange(x86_KSvtd_ریشه_جدول, VTD_RT_اندازه_بیتها);

    إذا (!vtd_enable(cpu_id)) {
        أرجع false;
    }
    أرجع true;
}

#پایان_اگر /* CONFIG_IOMMU */
