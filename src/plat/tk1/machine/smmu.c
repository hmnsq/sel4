/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <config.h>

#ifdef CONFIG_TK1_SMMU

#अन्तर्भाव <plat/machine/smmu.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <object/structures.h>


#define MC_PADDR                    0x70019000
#define SMMU_CONFIG_OFFSET          0x10

#define PTB_DATA_BASE_SHIFT         12
#define PTB_DATA_READ               BIT(31)
#define PTB_DATA_WRITE              BIT(30)
#define PTB_DATA_NONSECURE          BIT(29)
#define PTB_DATA_BASE_PD_MASK       0x3fffff

#define MODULE_ASID_ENABLE          BIT(31)

#define PTC_FLUSH_ALL               0
#define PTC_FLUSH_ADR               1

#define TLB_ASID_MATCH              BIT(31)
#define TLB_FLUSH_ALL               (0)
#define TLB_FLUSH_SECTION           (2)
#define TLB_FLUSH_GROUP             (3)

#define MC_DECERR_MTS_BIT           16u
#define MC_SECERR_SEC_BIT           13u
#define MC_DECERR_VPR_BIT           12u
#define MC_APB_ASID_UPDATE_BIT      11u
#define MC_SMMU_PAGE_BIT            10u
#define MC_ARBITRATION_EMEM_BIT     9u
#define MC_SECURITY_BIT             8u
#define MC_DECERR_EMEM_BIT          6u


#define MC_ERR_ID_MASK              0x7f
#define MC_ERR_ADR_MASK             0x7000
#define MC_ERR_RW_MASK              0x10000
#define MC_ERR_SEC_MASK             0x20000
#define MC_ERR_SWAP_MASK            0x40000
#define MC_ERR_ADR_HI_MASK          0x300000
#define MC_ERR_INVALID_SMMU_PAGE_NONSECURE_MASK     0x2000000
#define MC_ERR_INVALID_SMMU_PAGE_WRITE_MASK         0x4000000
#define MC_ERR_INVALID_SMMU_PAGE_READ_MASK          0x8000000
#define MC_ERR_TYPE_MASK                            0x70000000
#define MC_ERR_TYPE_SHIFT                           28

#define MC_ERR_TYPE_RSVD                0
#define MC_ERR_TYPE_DECERR_EMEM         2
#define MC_ERR_TYPE_SECURITY            3
#define MC_ERR_TYPE_SECURITY_CARVEOUT   4
#define MC_ERR_TYPE_INVALID_SMMU_PAGE   6

#define IOPDE_4M_INDEX_SHIFT            22

स्थिर चञ्चल tk1_mc_regs_t *smmu_regs = (चञ्चल tk1_mc_regs_t *)(SMMU_PPTR);

स्थिर वर्ण smmu_pds[ARM_PLAT_अङ्क_SMMU][BIT(SMMU_PD_INDEX_BITS)] ALIGN(BIT(SMMU_PD_INDEX_BITS));

स्थिर शून्य do_smmu_enable(शून्य)
{
    चञ्चल uint32_t *config = (चञ्चल uint32_t *)(MC_PADDR + SMMU_CONFIG_OFFSET);
    *config = 1;
}

स्थिर शून्य do_smmu_disable(शून्य)
{
    चञ्चल uint32_t *config = (चञ्चल uint32_t *)(MC_PADDR + SMMU_CONFIG_OFFSET);
    *config = 0;
}

स्थिर अन्तःस्थ शून्य smmu_disable(शून्य)
{
    यदि (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* in hyp mode, we need call the hook in monitor mode */
        /* we need physical address here */
        paddr_t addr = addrFromKPPtr(&do_smmu_disable);
        यंत्र(".arch_extension sec\n");
        यंत्र चञ्चल("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } अन्यथा {
        /* in secure mode, can enable it directly */
        smmu_regs->smmu_config = 0;
    }

    फर्काऊ;
}

स्थिर अन्तःस्थ शून्य smmu_enable(शून्य)
{
    यदि (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        paddr_t addr = addrFromKPPtr(&do_smmu_enable);
        यंत्र(".arch_extension sec\n");
        यंत्र चञ्चल("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } अन्यथा {
        smmu_regs->smmu_config = 1;
    }

    फर्काऊ;
}


स्थिर uint32_t make_ptb_data(uint32_t pd_base, bool_t पठ, bool_t लिख, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_base >> PTB_DATA_BASE_SHIFT);

    यदि (पठ) {
        ret |= PTB_DATA_READ;
    }
    यदि (लिख) {
        ret |= PTB_DATA_WRITE;
    }
    यदि (nonsecure) {
        ret |= PTB_DATA_NONSECURE;
    }

    फर्काऊ ret;
}

शून्य plat_smmu_ptc_flush_all(शून्य)
{
    uint32_t cmd = PTC_FLUSH_ALL;
    smmu_regs->smmu_ptc_flush = cmd;
}

शून्य plat_smmu_tlb_flush_all(शून्य)
{
    uint32_t cmd = TLB_FLUSH_ALL;
    smmu_regs->smmu_tlb_flush = cmd;
}

BOOT_CODE पूर्णाङ्क plat_smmu_init(शून्य)
{
    uint32_t asid;

    smmu_disable();

    लागि (asid = SMMU_FIRST_ASID; asid <= SMMU_LAST_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_FIRST_ASID];

        स_रखो(pd, 0, BIT(SMMU_PD_INDEX_BITS));
        cleanCacheRange_RAM((word_t)pd, ((word_t)pd + BIT(SMMU_PD_INDEX_BITS)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_data = make_ptb_data(pptr_to_paddr(pd), true, true, true);
    }
    म_लिखो("Total %d IOASID set up\n", (asid - 1));

    /* now assign IOASID to each module */
    smmu_regs->smmu_afi_asid = SMMU_AFI_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_avpc_asid = SMMU_AVPC_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_dc_asid = SMMU_DC_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_dcb_asid = SMMU_DCB_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_hc_asid = SMMU_HC_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_hda_asid = SMMU_HDA_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_isp2_asid = SMMU_ISP2_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_msenc_asid = SMMU_MSENC_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_nv_asid = SMMU_NV_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_nv2_asid = SMMU_NV2_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_ppcs_asid = SMMU_PPCS_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_sata_asid = SMMU_SATA_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_vde_asid = SMMU_VDE_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_vi_asid = SMMU_VI_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_vic_asid = SMMU_VIC_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_xusb_host_asid = SMMU_XUSB_HOST_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_xusb_dev_asid = SMMU_XUSB_DEV_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_tsec_asid = SMMU_TSEC_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_ppcs1_asid = SMMU_PPCS1_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_sdmmc1a_asid = SMMU_SDMMC1A_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_sdmmc2a_asid = SMMU_SDMMC2A_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_sdmmc3a_asid = SMMU_SDMMC3A_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_sdmmc4a_asid = SMMU_SDMMC4A_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_isp2b_asid = SMMU_ISP2B_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_gpu_asid = SMMU_GPU_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_gpub_asid = SMMU_GPUB_ASID | MODULE_ASID_ENABLE;
    smmu_regs->smmu_ppcs2_asid = SMMU_PPCS2_ASID | MODULE_ASID_ENABLE;

    /* flush page table cache */
    plat_smmu_ptc_flush_all();
    /* flush TLB              */
    plat_smmu_tlb_flush_all();
    smmu_enable();

    /* also need to unmask interrupts */
    यदि (config_set(CONFIG_SMMU_INTERRUPT_ENABLE)) {
        smmu_regs->intmask = BIT(MC_APB_ASID_UPDATE_BIT) | BIT(MC_SMMU_PAGE_BIT) |
                             BIT(MC_DECERR_MTS_BIT) | BIT(MC_SECERR_SEC_BIT) |
                             BIT(MC_DECERR_VPR_BIT) | BIT(MC_ARBITRATION_EMEM_BIT) |
                             BIT(MC_SECURITY_BIT) | BIT(MC_DECERR_EMEM_BIT);
    } अन्यथा {
        smmu_regs->intmask = 0;
    }
    फर्काऊ ARM_PLAT_अङ्क_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    प्रतिज्ञा(asid >= SMMU_FIRST_ASID && asid <= SMMU_LAST_ASID);
    फर्काऊ (iopde_t *) smmu_pds[asid - SMMU_FIRST_ASID];
}

शून्य plat_smmu_handle_interrupt(शून्य)
{
    uint32_t status = smmu_regs->intstatus;
    uint32_t clear_status = 0;

    यदि (status & BIT(MC_DECERR_MTS_BIT)) {
        clear_status |= BIT(MC_DECERR_MTS_BIT);
    }
    यदि (status & BIT(MC_SECERR_SEC_BIT)) {
        clear_status |= BIT(MC_SECERR_SEC_BIT);
    }
    यदि (status & BIT(MC_DECERR_VPR_BIT)) {
        clear_status |= BIT(MC_DECERR_VPR_BIT);
    }
    यदि (status & BIT(MC_ARBITRATION_EMEM_BIT)) {
        clear_status |= BIT(MC_ARBITRATION_EMEM_BIT);
    }
    यदि (status & BIT(MC_SECURITY_BIT)) {
        clear_status |= BIT(MC_SECURITY_BIT);
    }
    यदि (status & BIT(MC_DECERR_EMEM_BIT)) {
        clear_status |= BIT(MC_DECERR_EMEM_BIT);
    }
    यदि (status & BIT(MC_APB_ASID_UPDATE_BIT)) {
        clear_status |= BIT(MC_APB_ASID_UPDATE_BIT);
    }

    /* we only care about SMMU translation failures */
    यदि (status & BIT(MC_SMMU_PAGE_BIT)) {
        यदि (config_set(CONFIG_PRINTING)) {
            uint32_t err_status = smmu_regs->err_status;
            uint32_t UNUSED err_adr = smmu_regs->err_adr;
            uint32_t UNUSED id = err_status & MC_ERR_ID_MASK;
            uint32_t UNUSED rw = (err_status & MC_ERR_RW_MASK);
            uint32_t UNUSED पठ = (err_status & MC_ERR_INVALID_SMMU_PAGE_READ_MASK);
            uint32_t UNUSED लिख = (err_status & MC_ERR_INVALID_SMMU_PAGE_WRITE_MASK);
            uint32_t UNUSED nonsecure = (err_status & MC_ERR_INVALID_SMMU_PAGE_NONSECURE_MASK);
            uint32_t UNUSED type = (err_status & MC_ERR_TYPE_MASK) >> MC_ERR_TYPE_SHIFT;

            म_लिखो("SMMU Address translation error:\n");
            म_लिखो("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            म_लिखो("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", पठ, लिख, nonsecure);
        }
        clear_status |= BIT(MC_SMMU_PAGE_BIT);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_status;
}
#endif
