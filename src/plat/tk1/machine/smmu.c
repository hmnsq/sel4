/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU

#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/smmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_PADDR                    0x70019000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SMMU_CONFIG_ਵਿੱਥ          0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTB_ਅੰਕੜੇ_ਅਧਾਰ_ਖਿਸਕਾਓ         12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTB_ਅੰਕੜੇ_READ               ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTB_ਅੰਕੜੇ_WRITE              ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTB_ਅੰਕੜੇ_NONSECURE          ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTB_ਅੰਕੜੇ_ਅਧਾਰ_PD_ਨਕਾਬ       0x3fffff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MODULE_ASID_ENABLE          ਦੋਆਧਾਰੀ_ਅੰਕ(31)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTC_FLUSH_ALL               0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTC_FLUSH_ADR               1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TLB_ASID_ਮੇਲ              ਦੋਆਧਾਰੀ_ਅੰਕ(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TLB_FLUSH_ALL               (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TLB_FLUSH_SECTION           (2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TLB_FLUSH_ਸਮੂਹ             (3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_DECERR_MTS_ਦੋਆਧਾਰੀ_ਅੰਕ           16u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_SECERR_SEC_ਦੋਆਧਾਰੀ_ਅੰਕ           13u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_DECERR_VPR_ਦੋਆਧਾਰੀ_ਅੰਕ           12u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_APB_ASID_UPDATE_ਦੋਆਧਾਰੀ_ਅੰਕ      11u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_SMMU_ਪੰਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ            10u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ARBITRATION_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ     9u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_SECURITY_ਦੋਆਧਾਰੀ_ਅੰਕ             8u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_DECERR_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ          6u


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_ID_ਨਕਾਬ              0x7f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_ADR_ਨਕਾਬ             0x7000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_RW_ਨਕਾਬ              0x10000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_SEC_ਨਕਾਬ             0x20000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_ਅਦਲਾ_ਬਦਲੀ_ਨਕਾਬ            0x40000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_ADR_HI_ਨਕਾਬ          0x300000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_INVALID_SMMU_ਪੰਨਾ_NONSECURE_ਨਕਾਬ     0x2000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_INVALID_SMMU_ਪੰਨਾ_WRITE_ਨਕਾਬ         0x4000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_INVALID_SMMU_ਪੰਨਾ_READ_ਨਕਾਬ          0x8000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_ਨਕਾਬ                            0x70000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_ਖਿਸਕਾਓ                           28

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_RSVD                0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_DECERR_EMEM         2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_SECURITY            3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_SECURITY_CARVEOUT   4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MC_ERR_TYPE_INVALID_SMMU_ਪੰਨਾ   6

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOPDE_4_M_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ            22

ਸਥਿਰ ਅਸਥਿਰ tk1_mc_regs_t *smmu_regs = (ਅਸਥਿਰ tk1_mc_regs_t *)(SMMU_PPTR);

ਸਥਿਰ ਅੱਖਰ smmu_pds[ARM_PLAT_ਅੰਕ_SMMU][ਦੋਆਧਾਰੀ_ਅੰਕ(SMMU_PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(SMMU_PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ));

ਸਥਿਰ ਖਾਲੀ do_smmu_enable(ਖਾਲੀ)
{
    ਅਸਥਿਰ uint32_t *config = (ਅਸਥਿਰ uint32_t *)(MC_PADDR + SMMU_CONFIG_ਵਿੱਥ);
    *config = 1;
}

ਸਥਿਰ ਖਾਲੀ do_smmu_disable(ਖਾਲੀ)
{
    ਅਸਥਿਰ uint32_t *config = (ਅਸਥਿਰ uint32_t *)(MC_PADDR + SMMU_CONFIG_ਵਿੱਥ);
    *config = 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ smmu_disable(ਖਾਲੀ)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* in hyp mode, we need call the hook in monitor mode */
        /* we need physical address here */
        paddr_t addr = addrFromKPPtr(&do_smmu_disable);
        asm(".arch_extension sec\n");
        asm ਅਸਥਿਰ("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } ਨਹੀਂਤਾਂ {
        /* in secure mode, can enable it directly */
        smmu_regs->smmu_config = 0;
    }

    ਵਾਪਸ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ smmu_enable(ਖਾਲੀ)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        paddr_t addr = addrFromKPPtr(&do_smmu_enable);
        asm(".arch_extension sec\n");
        asm ਅਸਥਿਰ("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } ਨਹੀਂਤਾਂ {
        smmu_regs->smmu_config = 1;
    }

    ਵਾਪਸ;
}


ਸਥਿਰ uint32_t make_ptb_ਅੰਕੜੇ(uint32_t pd_ਅਧਾਰ, bool_t ਪਠ, bool_t ਲਿਖ, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_ਅਧਾਰ >> PTB_ਅੰਕੜੇ_ਅਧਾਰ_ਖਿਸਕਾਓ);

    ਜੇ (ਪਠ) {
        ret |= PTB_ਅੰਕੜੇ_READ;
    }
    ਜੇ (ਲਿਖ) {
        ret |= PTB_ਅੰਕੜੇ_WRITE;
    }
    ਜੇ (nonsecure) {
        ret |= PTB_ਅੰਕੜੇ_NONSECURE;
    }

    ਵਾਪਸ ret;
}

ਖਾਲੀ plat_smmu_ptc_flush_all(ਖਾਲੀ)
{
    uint32_t cmd = PTC_FLUSH_ALL;
    smmu_regs->smmu_ptc_flush = cmd;
}

ਖਾਲੀ plat_smmu_tlb_flush_all(ਖਾਲੀ)
{
    uint32_t cmd = TLB_FLUSH_ALL;
    smmu_regs->smmu_tlb_flush = cmd;
}

BOOT_ਕੋਡ ਪੂਰਨ plat_smmu_init(ਖਾਲੀ)
{
    uint32_t asid;

    smmu_disable();

    ਲਈ (asid = SMMU_ਪਹਿਲਾ_ASID; asid <= SMMU_ਆਖਰੀ_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_ਪਹਿਲਾ_ASID];

        memset(pd, 0, ਦੋਆਧਾਰੀ_ਅੰਕ(SMMU_PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
        cleanCacheRange_RAM((ਸ਼ਬਦ_t)pd, ((ਸ਼ਬਦ_t)pd + ਦੋਆਧਾਰੀ_ਅੰਕ(SMMU_PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_ਅੰਕੜੇ = make_ptb_ਅੰਕੜੇ(pptr_ਮੰਜ਼ਿਲ_paddr(pd), true, true, true);
    }
    ਲਿਖੋ("Total %d IOASID set up\n", (asid - 1));

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
    ਜੇ (config_ਸੈੱਟ(CONFIG_SMMU_ਰੁਕਾਵਟ_ENABLE)) {
        smmu_regs->intmask = ਦੋਆਧਾਰੀ_ਅੰਕ(MC_APB_ASID_UPDATE_ਦੋਆਧਾਰੀ_ਅੰਕ) | ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SMMU_ਪੰਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ) |
                             ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_MTS_ਦੋਆਧਾਰੀ_ਅੰਕ) | ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SECERR_SEC_ਦੋਆਧਾਰੀ_ਅੰਕ) |
                             ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_VPR_ਦੋਆਧਾਰੀ_ਅੰਕ) | ਦੋਆਧਾਰੀ_ਅੰਕ(MC_ARBITRATION_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ) |
                             ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SECURITY_ਦੋਆਧਾਰੀ_ਅੰਕ) | ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ);
    } ਨਹੀਂਤਾਂ {
        smmu_regs->intmask = 0;
    }
    ਵਾਪਸ ARM_PLAT_ਅੰਕ_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    ਪ੍ਰਤਿਜ੍ਞਾ(asid >= SMMU_ਪਹਿਲਾ_ASID && asid <= SMMU_ਆਖਰੀ_ASID);
    ਵਾਪਸ (iopde_t *) smmu_pds[asid - SMMU_ਪਹਿਲਾ_ASID];
}

ਖਾਲੀ plat_smmu_handle_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    uint32_t ਸਥਿਤੀ = smmu_regs->intstatus;
    uint32_t clear_ਸਥਿਤੀ = 0;

    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_MTS_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_MTS_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SECERR_SEC_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SECERR_SEC_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_VPR_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_VPR_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_ARBITRATION_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_ARBITRATION_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SECURITY_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SECURITY_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_DECERR_EMEM_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_APB_ASID_UPDATE_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_APB_ASID_UPDATE_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }

    /* we only care about SMMU translation failures */
    ਜੇ (ਸਥਿਤੀ & ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SMMU_ਪੰਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        ਜੇ (config_ਸੈੱਟ(CONFIG_PRINTING)) {
            uint32_t err_ਸਥਿਤੀ = smmu_regs->err_ਸਥਿਤੀ;
            uint32_t ਅਣਵਰਤਿਆ err_adr = smmu_regs->err_adr;
            uint32_t ਅਣਵਰਤਿਆ id = err_ਸਥਿਤੀ & MC_ERR_ID_ਨਕਾਬ;
            uint32_t ਅਣਵਰਤਿਆ rw = (err_ਸਥਿਤੀ & MC_ERR_RW_ਨਕਾਬ);
            uint32_t ਅਣਵਰਤਿਆ ਪਠ = (err_ਸਥਿਤੀ & MC_ERR_INVALID_SMMU_ਪੰਨਾ_READ_ਨਕਾਬ);
            uint32_t ਅਣਵਰਤਿਆ ਲਿਖ = (err_ਸਥਿਤੀ & MC_ERR_INVALID_SMMU_ਪੰਨਾ_WRITE_ਨਕਾਬ);
            uint32_t ਅਣਵਰਤਿਆ nonsecure = (err_ਸਥਿਤੀ & MC_ERR_INVALID_SMMU_ਪੰਨਾ_NONSECURE_ਨਕਾਬ);
            uint32_t ਅਣਵਰਤਿਆ type = (err_ਸਥਿਤੀ & MC_ERR_TYPE_ਨਕਾਬ) >> MC_ERR_TYPE_ਖਿਸਕਾਓ;

            ਲਿਖੋ("SMMU Address translation error:\n");
            ਲਿਖੋ("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            ਲਿਖੋ("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", ਪਠ, ਲਿਖ, nonsecure);
        }
        clear_ਸਥਿਤੀ |= ਦੋਆਧਾਰੀ_ਅੰਕ(MC_SMMU_ਪੰਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_ਸਥਿਤੀ;
}
#ਜੇ_ਅੰਤ
