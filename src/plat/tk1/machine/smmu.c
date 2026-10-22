/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU

#ಅನ್ತರ್ಭಾವ <plat/machine/smmu.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>


#ವ್ಯಾಖ್ಯಾನಿಸು MC_PADDR                    0x70019000
#ವ್ಯಾಖ್ಯಾನಿಸು SMMU_CONFIG_ಸ್ಥಾನಾಂತರ          0x10

#ವ್ಯಾಖ್ಯಾನಿಸು PTB_ದತ್ತಾಂಶ_ತಳ_ಸರಿಸು         12
#ವ್ಯಾಖ್ಯಾನಿಸು PTB_ದತ್ತಾಂಶ_READ               ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು PTB_ದತ್ತಾಂಶ_WRITE              ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು PTB_ದತ್ತಾಂಶ_NONSECURE          ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು PTB_ದತ್ತಾಂಶ_ತಳ_PD_ಮುಖವಾಡ       0x3fffff

#ವ್ಯಾಖ್ಯಾನಿಸು MODULE_ASID_ENABLE          ಬಿಟ್(31)

#ವ್ಯಾಖ್ಯಾನಿಸು PTC_FLUSH_ALL               0
#ವ್ಯಾಖ್ಯಾನಿಸು PTC_FLUSH_ADR               1

#ವ್ಯಾಖ್ಯಾನಿಸು TLB_ASID_ಹೊಂದಿಕೆ              ಬಿಟ್(31)
#ವ್ಯಾಖ್ಯಾನಿಸು TLB_FLUSH_ALL               (0)
#ವ್ಯಾಖ್ಯಾನಿಸು TLB_FLUSH_SECTION           (2)
#ವ್ಯಾಖ್ಯಾನಿಸು TLB_FLUSH_ಗುಂಪು             (3)

#ವ್ಯಾಖ್ಯಾನಿಸು MC_DECERR_MTS_ಬಿಟ್           16u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_SECERR_SEC_ಬಿಟ್           13u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_DECERR_VPR_ಬಿಟ್           12u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_APB_ASID_UPDATE_ಬಿಟ್      11u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_SMMU_ಪುಟ_ಬಿಟ್            10u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ARBITRATION_EMEM_ಬಿಟ್     9u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_SECURITY_ಬಿಟ್             8u
#ವ್ಯಾಖ್ಯಾನಿಸು MC_DECERR_EMEM_ಬಿಟ್          6u


#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_ID_ಮುಖವಾಡ              0x7f
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_ADR_ಮುಖವಾಡ             0x7000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_RW_ಮುಖವಾಡ              0x10000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_SEC_ಮುಖವಾಡ             0x20000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_ಅದಲುಬದಲು_ಮುಖವಾಡ            0x40000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_ADR_HI_ಮುಖವಾಡ          0x300000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_INVALID_SMMU_ಪುಟ_NONSECURE_ಮುಖವಾಡ     0x2000000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_INVALID_SMMU_ಪುಟ_WRITE_ಮುಖವಾಡ         0x4000000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_INVALID_SMMU_ಪುಟ_READ_ಮುಖವಾಡ          0x8000000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_ಮುಖವಾಡ                            0x70000000
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_ಸರಿಸು                           28

#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_RSVD                0
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_DECERR_EMEM         2
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_SECURITY            3
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_SECURITY_CARVEOUT   4
#ವ್ಯಾಖ್ಯಾನಿಸು MC_ERR_TYPE_INVALID_SMMU_ಪುಟ   6

#ವ್ಯಾಖ್ಯಾನಿಸು IOPDE_4_M_ಸೂಚ್ಯಂಕ_ಸರಿಸು            22

ಸ್ಥಿರ ಅಸ್ಥಿರ tk1_mc_regs_t *smmu_regs = (ಅಸ್ಥಿರ tk1_mc_regs_t *)(SMMU_PPTR);

ಸ್ಥಿರ ಅಕ್ಷರ smmu_pds[ARM_PLAT_ಅಂಕೆ_SMMU][ಬಿಟ್(SMMU_PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(SMMU_PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));

ಸ್ಥಿರ ಶೂನ್ಯ do_smmu_enable(ಶೂನ್ಯ)
{
    ಅಸ್ಥಿರ uint32_t *config = (ಅಸ್ಥಿರ uint32_t *)(MC_PADDR + SMMU_CONFIG_ಸ್ಥಾನಾಂತರ);
    *config = 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ do_smmu_disable(ಶೂನ್ಯ)
{
    ಅಸ್ಥಿರ uint32_t *config = (ಅಸ್ಥಿರ uint32_t *)(MC_PADDR + SMMU_CONFIG_ಸ್ಥಾನಾಂತರ);
    *config = 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ smmu_disable(ಶೂನ್ಯ)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* in hyp mode, we need call the hook in monitor mode */
        /* we need physical address here */
        paddr_t addr = addrFromKPPtr(&do_smmu_disable);
        asm(".arch_extension sec\n");
        asm ಅಸ್ಥಿರ("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* in secure mode, can enable it directly */
        smmu_regs->smmu_config = 0;
    }

    ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ smmu_enable(ಶೂನ್ಯ)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        paddr_t addr = addrFromKPPtr(&do_smmu_enable);
        asm(".arch_extension sec\n");
        asm ಅಸ್ಥಿರ("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } ಇಲ್ಲದಿದ್ದರೆ {
        smmu_regs->smmu_config = 1;
    }

    ಹಿಂತಿರುಗಿ;
}


ಸ್ಥಿರ uint32_t make_ptb_ದತ್ತಾಂಶ(uint32_t pd_ತಳ, bool_t ಪಠ, bool_t ಲಿಖ, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_ತಳ >> PTB_ದತ್ತಾಂಶ_ತಳ_ಸರಿಸು);

    ಆದರೆ (ಪಠ) {
        ret |= PTB_ದತ್ತಾಂಶ_READ;
    }
    ಆದರೆ (ಲಿಖ) {
        ret |= PTB_ದತ್ತಾಂಶ_WRITE;
    }
    ಆದರೆ (nonsecure) {
        ret |= PTB_ದತ್ತಾಂಶ_NONSECURE;
    }

    ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ plat_smmu_ptc_flush_all(ಶೂನ್ಯ)
{
    uint32_t cmd = PTC_FLUSH_ALL;
    smmu_regs->smmu_ptc_flush = cmd;
}

ಶೂನ್ಯ plat_smmu_tlb_flush_all(ಶೂನ್ಯ)
{
    uint32_t cmd = TLB_FLUSH_ALL;
    smmu_regs->smmu_tlb_flush = cmd;
}

BOOT_ಸಂಕೇತ ಪೂರ್ಣಾಂಕ plat_smmu_init(ಶೂನ್ಯ)
{
    uint32_t asid;

    smmu_disable();

    ಗಾಗಿ (asid = SMMU_ಮೊದಲ_ASID; asid <= SMMU_ಕೊನೆಯ_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_ಮೊದಲ_ASID];

        memset(pd, 0, ಬಿಟ್(SMMU_PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
        cleanCacheRange_RAM((ಶಬ್ದ_t)pd, ((ಶಬ್ದ_t)pd + ಬಿಟ್(SMMU_PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_ದತ್ತಾಂಶ = make_ptb_ದತ್ತಾಂಶ(pptr_ಗಮ್ಯ_paddr(pd), true, true, true);
    }
    ಬರೆ("Total %d IOASID set up\n", (asid - 1));

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
    ಆದರೆ (config_ಗಣ(CONFIG_SMMU_ಅಡಚಣೆ_ENABLE)) {
        smmu_regs->intmask = ಬಿಟ್(MC_APB_ASID_UPDATE_ಬಿಟ್) | ಬಿಟ್(MC_SMMU_ಪುಟ_ಬಿಟ್) |
                             ಬಿಟ್(MC_DECERR_MTS_ಬಿಟ್) | ಬಿಟ್(MC_SECERR_SEC_ಬಿಟ್) |
                             ಬಿಟ್(MC_DECERR_VPR_ಬಿಟ್) | ಬಿಟ್(MC_ARBITRATION_EMEM_ಬಿಟ್) |
                             ಬಿಟ್(MC_SECURITY_ಬಿಟ್) | ಬಿಟ್(MC_DECERR_EMEM_ಬಿಟ್);
    } ಇಲ್ಲದಿದ್ದರೆ {
        smmu_regs->intmask = 0;
    }
    ಹಿಂತಿರುಗಿ ARM_PLAT_ಅಂಕೆ_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    ಪ್ರತಿಜ್ಞಾ(asid >= SMMU_ಮೊದಲ_ASID && asid <= SMMU_ಕೊನೆಯ_ASID);
    ಹಿಂತಿರುಗಿ (iopde_t *) smmu_pds[asid - SMMU_ಮೊದಲ_ASID];
}

ಶೂನ್ಯ plat_smmu_handle_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    uint32_t ಸ್ಥಿತಿ_2 = smmu_regs->intstatus;
    uint32_t clear_ಸ್ಥಿತಿ = 0;

    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_DECERR_MTS_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_DECERR_MTS_ಬಿಟ್);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_SECERR_SEC_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_SECERR_SEC_ಬಿಟ್);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_DECERR_VPR_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_DECERR_VPR_ಬಿಟ್);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_ARBITRATION_EMEM_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_ARBITRATION_EMEM_ಬಿಟ್);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_SECURITY_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_SECURITY_ಬಿಟ್);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_DECERR_EMEM_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_DECERR_EMEM_ಬಿಟ್);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_APB_ASID_UPDATE_ಬಿಟ್)) {
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_APB_ASID_UPDATE_ಬಿಟ್);
    }

    /* we only care about SMMU translation failures */
    ಆದರೆ (ಸ್ಥಿತಿ_2 & ಬಿಟ್(MC_SMMU_ಪುಟ_ಬಿಟ್)) {
        ಆದರೆ (config_ಗಣ(CONFIG_PRINTING)) {
            uint32_t err_ಸ್ಥಿತಿ = smmu_regs->err_ಸ್ಥಿತಿ;
            uint32_t ಬಳಸದ err_adr = smmu_regs->err_adr;
            uint32_t ಬಳಸದ id = err_ಸ್ಥಿತಿ & MC_ERR_ID_ಮುಖವಾಡ;
            uint32_t ಬಳಸದ rw = (err_ಸ್ಥಿತಿ & MC_ERR_RW_ಮುಖವಾಡ);
            uint32_t ಬಳಸದ ಪಠ = (err_ಸ್ಥಿತಿ & MC_ERR_INVALID_SMMU_ಪುಟ_READ_ಮುಖವಾಡ);
            uint32_t ಬಳಸದ ಲಿಖ = (err_ಸ್ಥಿತಿ & MC_ERR_INVALID_SMMU_ಪುಟ_WRITE_ಮುಖವಾಡ);
            uint32_t ಬಳಸದ nonsecure = (err_ಸ್ಥಿತಿ & MC_ERR_INVALID_SMMU_ಪುಟ_NONSECURE_ಮುಖವಾಡ);
            uint32_t ಬಳಸದ type = (err_ಸ್ಥಿತಿ & MC_ERR_TYPE_ಮುಖವಾಡ) >> MC_ERR_TYPE_ಸರಿಸು;

            ಬರೆ("SMMU Address translation error:\n");
            ಬರೆ("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            ಬರೆ("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", ಪಠ, ಲಿಖ, nonsecure);
        }
        clear_ಸ್ಥಿತಿ |= ಬಿಟ್(MC_SMMU_ಪುಟ_ಬಿಟ್);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_ಸ್ಥಿತಿ;
}
#ಆದರೆ_ಅಂತ್ಯ
