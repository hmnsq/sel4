/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_TK1_SMMU

#అన్తర్భావ <plat/machine/smmu.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <object/structures.h>


#నిర్వచించు MC_PADDR                    0x70019000
#నిర్వచించు SMMU_CONFIG_స్థానభ్రంశం          0x10

#నిర్వచించు PTB_దత్తాంశం_పునాది_జరుపు         12
#నిర్వచించు PTB_దత్తాంశం_READ               బిట్టు(31)
#నిర్వచించు PTB_దత్తాంశం_WRITE              బిట్టు(30)
#నిర్వచించు PTB_దత్తాంశం_NONSECURE          బిట్టు(29)
#నిర్వచించు PTB_దత్తాంశం_పునాది_PD_ముసుగు       0x3fffff

#నిర్వచించు MODULE_ASID_ENABLE          బిట్టు(31)

#నిర్వచించు PTC_FLUSH_ALL               0
#నిర్వచించు PTC_FLUSH_ADR               1

#నిర్వచించు TLB_ASID_సరిపోలిక              బిట్టు(31)
#నిర్వచించు TLB_FLUSH_ALL               (0)
#నిర్వచించు TLB_FLUSH_SECTION           (2)
#నిర్వచించు TLB_FLUSH_సమూహం             (3)

#నిర్వచించు MC_DECERR_MTS_బిట్టు           16u
#నిర్వచించు MC_SECERR_SEC_బిట్టు           13u
#నిర్వచించు MC_DECERR_VPR_బిట్టు           12u
#నిర్వచించు MC_APB_ASID_UPDATE_బిట్టు      11u
#నిర్వచించు MC_SMMU_పుట_బిట్టు            10u
#నిర్వచించు MC_ARBITRATION_EMEM_బిట్టు     9u
#నిర్వచించు MC_SECURITY_బిట్టు             8u
#నిర్వచించు MC_DECERR_EMEM_బిట్టు          6u


#నిర్వచించు MC_ERR_ID_ముసుగు              0x7f
#నిర్వచించు MC_ERR_ADR_ముసుగు             0x7000
#నిర్వచించు MC_ERR_RW_ముసుగు              0x10000
#నిర్వచించు MC_ERR_SEC_ముసుగు             0x20000
#నిర్వచించు MC_ERR_మార్పిడి_ముసుగు            0x40000
#నిర్వచించు MC_ERR_ADR_HI_ముసుగు          0x300000
#నిర్వచించు MC_ERR_INVALID_SMMU_పుట_NONSECURE_ముసుగు     0x2000000
#నిర్వచించు MC_ERR_INVALID_SMMU_పుట_WRITE_ముసుగు         0x4000000
#నిర్వచించు MC_ERR_INVALID_SMMU_పుట_READ_ముసుగు          0x8000000
#నిర్వచించు MC_ERR_TYPE_ముసుగు                            0x70000000
#నిర్వచించు MC_ERR_TYPE_జరుపు                           28

#నిర్వచించు MC_ERR_TYPE_RSVD                0
#నిర్వచించు MC_ERR_TYPE_DECERR_EMEM         2
#నిర్వచించు MC_ERR_TYPE_SECURITY            3
#నిర్వచించు MC_ERR_TYPE_SECURITY_CARVEOUT   4
#నిర్వచించు MC_ERR_TYPE_INVALID_SMMU_పుట   6

#నిర్వచించు IOPDE_4_M_సూచీ_జరుపు            22

స్థిర అస్థిర tk1_mc_regs_t *smmu_regs = (అస్థిర tk1_mc_regs_t *)(SMMU_PPTR);

స్థిర అక్షరం smmu_pds[ARM_PLAT_అంకె_SMMU][బిట్టు(SMMU_PD_సూచీ_బిట్లు)] ALIGN(బిట్టు(SMMU_PD_సూచీ_బిట్లు));

స్థిర శూన్య do_smmu_enable(శూన్య)
{
    అస్థిర uint32_t *config = (అస్థిర uint32_t *)(MC_PADDR + SMMU_CONFIG_స్థానభ్రంశం);
    *config = 1;
}

స్థిర శూన్య do_smmu_disable(శూన్య)
{
    అస్థిర uint32_t *config = (అస్థిర uint32_t *)(MC_PADDR + SMMU_CONFIG_స్థానభ్రంశం);
    *config = 0;
}

స్థిర అంతఃవరుస శూన్య smmu_disable(శూన్య)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* in hyp mode, we need call the hook in monitor mode */
        /* we need physical address here */
        paddr_t addr = addrFromKPPtr(&do_smmu_disable);
        asm(".arch_extension sec\n");
        asm అస్థిర("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } లేకపోతే {
        /* in secure mode, can enable it directly */
        smmu_regs->smmu_config = 0;
    }

    తిరిగి;
}

స్థిర అంతఃవరుస శూన్య smmu_enable(శూన్య)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        paddr_t addr = addrFromKPPtr(&do_smmu_enable);
        asm(".arch_extension sec\n");
        asm అస్థిర("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } లేకపోతే {
        smmu_regs->smmu_config = 1;
    }

    తిరిగి;
}


స్థిర uint32_t make_ptb_దత్తాంశం(uint32_t pd_పునాది, bool_t పఠ, bool_t లిఖ, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_పునాది >> PTB_దత్తాంశం_పునాది_జరుపు);

    ఒకవేళ (పఠ) {
        ret |= PTB_దత్తాంశం_READ;
    }
    ఒకవేళ (లిఖ) {
        ret |= PTB_దత్తాంశం_WRITE;
    }
    ఒకవేళ (nonsecure) {
        ret |= PTB_దత్తాంశం_NONSECURE;
    }

    తిరిగి ret;
}

శూన్య plat_smmu_ptc_flush_all(శూన్య)
{
    uint32_t cmd = PTC_FLUSH_ALL;
    smmu_regs->smmu_ptc_flush = cmd;
}

శూన్య plat_smmu_tlb_flush_all(శూన్య)
{
    uint32_t cmd = TLB_FLUSH_ALL;
    smmu_regs->smmu_tlb_flush = cmd;
}

BOOT_సంకేతం పూర్ణాంకం plat_smmu_init(శూన్య)
{
    uint32_t asid;

    smmu_disable();

    కొరకు (asid = SMMU_మొదటి_ASID; asid <= SMMU_చివరి_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_మొదటి_ASID];

        memset(pd, 0, బిట్టు(SMMU_PD_సూచీ_బిట్లు));
        cleanCacheRange_RAM((మాట_t)pd, ((మాట_t)pd + బిట్టు(SMMU_PD_సూచీ_బిట్లు)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_దత్తాంశం = make_ptb_దత్తాంశం(pptr_గమ్యం_paddr(pd), true, true, true);
    }
    రాయి("Total %d IOASID set up\n", (asid - 1));

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
    ఒకవేళ (config_సమితి(CONFIG_SMMU_అంతరాయం_ENABLE)) {
        smmu_regs->intmask = బిట్టు(MC_APB_ASID_UPDATE_బిట్టు) | బిట్టు(MC_SMMU_పుట_బిట్టు) |
                             బిట్టు(MC_DECERR_MTS_బిట్టు) | బిట్టు(MC_SECERR_SEC_బిట్టు) |
                             బిట్టు(MC_DECERR_VPR_బిట్టు) | బిట్టు(MC_ARBITRATION_EMEM_బిట్టు) |
                             బిట్టు(MC_SECURITY_బిట్టు) | బిట్టు(MC_DECERR_EMEM_బిట్టు);
    } లేకపోతే {
        smmu_regs->intmask = 0;
    }
    తిరిగి ARM_PLAT_అంకె_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    ప్రతిజ్ఞా(asid >= SMMU_మొదటి_ASID && asid <= SMMU_చివరి_ASID);
    తిరిగి (iopde_t *) smmu_pds[asid - SMMU_మొదటి_ASID];
}

శూన్య plat_smmu_handle_అంతరాయం(శూన్య)
{
    uint32_t స్థితి_2 = smmu_regs->intstatus;
    uint32_t clear_స్థితి = 0;

    ఒకవేళ (స్థితి_2 & బిట్టు(MC_DECERR_MTS_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_DECERR_MTS_బిట్టు);
    }
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_SECERR_SEC_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_SECERR_SEC_బిట్టు);
    }
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_DECERR_VPR_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_DECERR_VPR_బిట్టు);
    }
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_ARBITRATION_EMEM_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_ARBITRATION_EMEM_బిట్టు);
    }
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_SECURITY_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_SECURITY_బిట్టు);
    }
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_DECERR_EMEM_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_DECERR_EMEM_బిట్టు);
    }
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_APB_ASID_UPDATE_బిట్టు)) {
        clear_స్థితి |= బిట్టు(MC_APB_ASID_UPDATE_బిట్టు);
    }

    /* we only care about SMMU translation failures */
    ఒకవేళ (స్థితి_2 & బిట్టు(MC_SMMU_పుట_బిట్టు)) {
        ఒకవేళ (config_సమితి(CONFIG_PRINTING)) {
            uint32_t err_స్థితి = smmu_regs->err_స్థితి;
            uint32_t వాడని err_adr = smmu_regs->err_adr;
            uint32_t వాడని id = err_స్థితి & MC_ERR_ID_ముసుగు;
            uint32_t వాడని rw = (err_స్థితి & MC_ERR_RW_ముసుగు);
            uint32_t వాడని పఠ = (err_స్థితి & MC_ERR_INVALID_SMMU_పుట_READ_ముసుగు);
            uint32_t వాడని లిఖ = (err_స్థితి & MC_ERR_INVALID_SMMU_పుట_WRITE_ముసుగు);
            uint32_t వాడని nonsecure = (err_స్థితి & MC_ERR_INVALID_SMMU_పుట_NONSECURE_ముసుగు);
            uint32_t వాడని type = (err_స్థితి & MC_ERR_TYPE_ముసుగు) >> MC_ERR_TYPE_జరుపు;

            రాయి("SMMU Address translation error:\n");
            రాయి("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            రాయి("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", పఠ, లిఖ, nonsecure);
        }
        clear_స్థితి |= బిట్టు(MC_SMMU_పుట_బిట్టు);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_స్థితి;
}
#అయితే_ముగింపు
