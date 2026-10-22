/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_TK1_SMMU

#अन्तर्भाव <plat/machine/smmu.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <object/structures.h>


#परिभाषय MC_PADDR                    0x70019000
#परिभाषय SMMU_CONFIG_विस्थापनम्          0x10

#परिभाषय PTB_दत्त_आधारः_सरणम्         12
#परिभाषय PTB_दत्त_READ               BIT(31)
#परिभाषय PTB_दत्त_WRITE              BIT(30)
#परिभाषय PTB_दत्त_NONSECURE          BIT(29)
#परिभाषय PTB_दत्त_आधारः_PD_आवरणम्       0x3fffff

#परिभाषय MODULE_ASID_ENABLE          BIT(31)

#परिभाषय PTC_FLUSH_ALL               0
#परिभाषय PTC_FLUSH_ADR               1

#परिभाषय TLB_ASID_मेलनम्              BIT(31)
#परिभाषय TLB_FLUSH_ALL               (0)
#परिभाषय TLB_FLUSH_SECTION           (2)
#परिभाषय TLB_FLUSH_गणः             (3)

#परिभाषय MC_DECERR_MTS_BIT           16u
#परिभाषय MC_SECERR_SEC_BIT           13u
#परिभाषय MC_DECERR_VPR_BIT           12u
#परिभाषय MC_APB_ASID_UPDATE_BIT      11u
#परिभाषय MC_SMMU_PAGE_BIT            10u
#परिभाषय MC_ARBITRATION_EMEM_BIT     9u
#परिभाषय MC_SECURITY_BIT             8u
#परिभाषय MC_DECERR_EMEM_BIT          6u


#परिभाषय MC_ERR_ID_आवरणम्              0x7f
#परिभाषय MC_ERR_ADR_आवरणम्             0x7000
#परिभाषय MC_ERR_RW_आवरणम्              0x10000
#परिभाषय MC_ERR_SEC_आवरणम्             0x20000
#परिभाषय MC_ERR_SWAP_आवरणम्            0x40000
#परिभाषय MC_ERR_ADR_HI_आवरणम्          0x300000
#परिभाषय MC_ERR_INVALID_SMMU_PAGE_NONSECURE_आवरणम्     0x2000000
#परिभाषय MC_ERR_INVALID_SMMU_PAGE_WRITE_आवरणम्         0x4000000
#परिभाषय MC_ERR_INVALID_SMMU_PAGE_READ_आवरणम्          0x8000000
#परिभाषय MC_ERR_TYPE_आवरणम्                            0x70000000
#परिभाषय MC_ERR_TYPE_सरणम्                           28

#परिभाषय MC_ERR_TYPE_RSVD                0
#परिभाषय MC_ERR_TYPE_DECERR_EMEM         2
#परिभाषय MC_ERR_TYPE_SECURITY            3
#परिभाषय MC_ERR_TYPE_SECURITY_CARVEOUT   4
#परिभाषय MC_ERR_TYPE_INVALID_SMMU_PAGE   6

#परिभाषय IOPDE_4_M_सूचकांक_सरणम्            22

स्थिर चञ्चल tk1_mc_regs_t *smmu_regs = (चञ्चल tk1_mc_regs_t *)(SMMU_PPTR);

स्थिर वर्ण smmu_pds[ARM_PLAT_अङ्कः_SMMU][BIT(SMMU_PD_सूचकांक_द्वयङ्काः)] ALIGN(BIT(SMMU_PD_सूचकांक_द्वयङ्काः));

स्थिर शून्य do_smmu_enable(शून्य)
{
    चञ्चल uint32_t *config = (चञ्चल uint32_t *)(MC_PADDR + SMMU_CONFIG_विस्थापनम्);
    *config = 1;
}

स्थिर शून्य do_smmu_disable(शून्य)
{
    चञ्चल uint32_t *config = (चञ्चल uint32_t *)(MC_PADDR + SMMU_CONFIG_विस्थापनम्);
    *config = 0;
}

स्थिर अन्तःस्थ शून्य smmu_disable(शून्य)
{
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* in hyp mode, we need call the hook in monitor mode */
        /* we need physical address here */
        paddr_t addr = addrFromKPPtr(&do_smmu_disable);
        asm(".arch_extension sec\n");
        asm चञ्चल("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } अन्यथा {
        /* in secure mode, can enable it directly */
        smmu_regs->smmu_config = 0;
    }

    प्रत्यागम;
}

स्थिर अन्तःस्थ शून्य smmu_enable(शून्य)
{
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        paddr_t addr = addrFromKPPtr(&do_smmu_enable);
        asm(".arch_extension sec\n");
        asm चञ्चल("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } अन्यथा {
        smmu_regs->smmu_config = 1;
    }

    प्रत्यागम;
}


स्थिर uint32_t make_ptb_दत्त(uint32_t pd_आधारः, bool_t पठ, bool_t लिख, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_आधारः >> PTB_दत्त_आधारः_सरणम्);

    यदि (पठ) {
        ret |= PTB_दत्त_READ;
    }
    यदि (लिख) {
        ret |= PTB_दत्त_WRITE;
    }
    यदि (nonsecure) {
        ret |= PTB_दत्त_NONSECURE;
    }

    प्रत्यागम ret;
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

BOOT_संहिता पूर्णाङ्क plat_smmu_init(शून्य)
{
    uint32_t asid;

    smmu_disable();

    प्रति (asid = SMMU_FIRST_ASID; asid <= SMMU_चरमः_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_FIRST_ASID];

        स्मृतिं_पूरय(pd, 0, BIT(SMMU_PD_सूचकांक_द्वयङ्काः));
        cleanCacheRange_RAM((word_t)pd, ((word_t)pd + BIT(SMMU_PD_सूचकांक_द्वयङ्काः)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_दत्त = make_ptb_दत्त(pptr_गन्तव्यम्_paddr(pd), true, true, true);
    }
    प्रारूप_लिख("Total %d IOASID set up\n", (asid - 1));

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
    यदि (config_समुच्चयः(CONFIG_SMMU_व्यवधानम्_ENABLE)) {
        smmu_regs->intmask = BIT(MC_APB_ASID_UPDATE_BIT) | BIT(MC_SMMU_PAGE_BIT) |
                             BIT(MC_DECERR_MTS_BIT) | BIT(MC_SECERR_SEC_BIT) |
                             BIT(MC_DECERR_VPR_BIT) | BIT(MC_ARBITRATION_EMEM_BIT) |
                             BIT(MC_SECURITY_BIT) | BIT(MC_DECERR_EMEM_BIT);
    } अन्यथा {
        smmu_regs->intmask = 0;
    }
    प्रत्यागम ARM_PLAT_अङ्कः_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    प्रतिज्ञा(asid >= SMMU_FIRST_ASID && asid <= SMMU_चरमः_ASID);
    प्रत्यागम (iopde_t *) smmu_pds[asid - SMMU_FIRST_ASID];
}

शून्य plat_smmu_handle_व्यवधानम्(शून्य)
{
    uint32_t स्थितिः = smmu_regs->intstatus;
    uint32_t clear_स्थितिः = 0;

    यदि (स्थितिः & BIT(MC_DECERR_MTS_BIT)) {
        clear_स्थितिः |= BIT(MC_DECERR_MTS_BIT);
    }
    यदि (स्थितिः & BIT(MC_SECERR_SEC_BIT)) {
        clear_स्थितिः |= BIT(MC_SECERR_SEC_BIT);
    }
    यदि (स्थितिः & BIT(MC_DECERR_VPR_BIT)) {
        clear_स्थितिः |= BIT(MC_DECERR_VPR_BIT);
    }
    यदि (स्थितिः & BIT(MC_ARBITRATION_EMEM_BIT)) {
        clear_स्थितिः |= BIT(MC_ARBITRATION_EMEM_BIT);
    }
    यदि (स्थितिः & BIT(MC_SECURITY_BIT)) {
        clear_स्थितिः |= BIT(MC_SECURITY_BIT);
    }
    यदि (स्थितिः & BIT(MC_DECERR_EMEM_BIT)) {
        clear_स्थितिः |= BIT(MC_DECERR_EMEM_BIT);
    }
    यदि (स्थितिः & BIT(MC_APB_ASID_UPDATE_BIT)) {
        clear_स्थितिः |= BIT(MC_APB_ASID_UPDATE_BIT);
    }

    /* we only care about SMMU translation failures */
    यदि (स्थितिः & BIT(MC_SMMU_PAGE_BIT)) {
        यदि (config_समुच्चयः(CONFIG_PRINTING)) {
            uint32_t err_स्थितिः = smmu_regs->err_स्थितिः;
            uint32_t अप्रयुक्तम् err_adr = smmu_regs->err_adr;
            uint32_t अप्रयुक्तम् id = err_स्थितिः & MC_ERR_ID_आवरणम्;
            uint32_t अप्रयुक्तम् rw = (err_स्थितिः & MC_ERR_RW_आवरणम्);
            uint32_t अप्रयुक्तम् पठ = (err_स्थितिः & MC_ERR_INVALID_SMMU_PAGE_READ_आवरणम्);
            uint32_t अप्रयुक्तम् लिख = (err_स्थितिः & MC_ERR_INVALID_SMMU_PAGE_WRITE_आवरणम्);
            uint32_t अप्रयुक्तम् nonsecure = (err_स्थितिः & MC_ERR_INVALID_SMMU_PAGE_NONSECURE_आवरणम्);
            uint32_t अप्रयुक्तम् type = (err_स्थितिः & MC_ERR_TYPE_आवरणम्) >> MC_ERR_TYPE_सरणम्;

            प्रारूप_लिख("SMMU Address translation error:\n");
            प्रारूप_लिख("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            प्रारूप_लिख("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", पठ, लिख, nonsecure);
        }
        clear_स्थितिः |= BIT(MC_SMMU_PAGE_BIT);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_स्थितिः;
}
#यदि_अन्तः
