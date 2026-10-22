/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <config.h>

#که_تعریف_شوی CONFIG_TK1_SMMU

#ضمن <plat/machine/smmu.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <object/structures.h>


#تعریف_کړه MC_PADDR                    0x70019000
#تعریف_کړه SMMU_CONFIG_دننه_واټن          0x10

#تعریف_کړه PTB_معلومات_بنسټ_ښويدنه         12
#تعریف_کړه PTB_معلومات_READ               بټ(31)
#تعریف_کړه PTB_معلومات_WRITE              بټ(30)
#تعریف_کړه PTB_معلومات_NONSECURE          بټ(29)
#تعریف_کړه PTB_معلومات_بنسټ_PD_ماسک       0x3fffff

#تعریف_کړه MODULE_ASID_ENABLE          بټ(31)

#تعریف_کړه PTC_FLUSH_ALL               0
#تعریف_کړه PTC_FLUSH_ADR               1

#تعریف_کړه TLB_ASID_سمون              بټ(31)
#تعریف_کړه TLB_FLUSH_ALL               (0)
#تعریف_کړه TLB_FLUSH_SECTION           (2)
#تعریف_کړه TLB_FLUSH_ډله             (3)

#تعریف_کړه MC_DECERR_MTS_بټ           16u
#تعریف_کړه MC_SECERR_SEC_بټ           13u
#تعریف_کړه MC_DECERR_VPR_بټ           12u
#تعریف_کړه MC_APB_ASID_UPDATE_بټ      11u
#تعریف_کړه MC_SMMU_پاڼه_بټ            10u
#تعریف_کړه MC_ARBITRATION_EMEM_بټ     9u
#تعریف_کړه MC_SECURITY_بټ             8u
#تعریف_کړه MC_DECERR_EMEM_بټ          6u


#تعریف_کړه MC_ERR_ID_ماسک              0x7f
#تعریف_کړه MC_ERR_ADR_ماسک             0x7000
#تعریف_کړه MC_ERR_RW_ماسک              0x10000
#تعریف_کړه MC_ERR_SEC_ماسک             0x20000
#تعریف_کړه MC_ERR_بدلون_ماسک            0x40000
#تعریف_کړه MC_ERR_ADR_HI_ماسک          0x300000
#تعریف_کړه MC_ERR_INVALID_SMMU_پاڼه_NONSECURE_ماسک     0x2000000
#تعریف_کړه MC_ERR_INVALID_SMMU_پاڼه_WRITE_ماسک         0x4000000
#تعریف_کړه MC_ERR_INVALID_SMMU_پاڼه_READ_ماسک          0x8000000
#تعریف_کړه MC_ERR_TYPE_ماسک                            0x70000000
#تعریف_کړه MC_ERR_TYPE_ښويدنه                           28

#تعریف_کړه MC_ERR_TYPE_RSVD                0
#تعریف_کړه MC_ERR_TYPE_DECERR_EMEM         2
#تعریف_کړه MC_ERR_TYPE_SECURITY            3
#تعریف_کړه MC_ERR_TYPE_SECURITY_CARVEOUT   4
#تعریف_کړه MC_ERR_TYPE_INVALID_SMMU_پاڼه   6

#تعریف_کړه IOPDE_4_M_شاخص_ښويدنه            22

ساكن متطاير tk1_mc_regs_t *smmu_regs = (متطاير tk1_mc_regs_t *)(SMMU_PPTR);

ساكن حرف smmu_pds[ARM_PLAT_عدد_SMMU][بټ(SMMU_PD_شاخص_بټونه)] ALIGN(بټ(SMMU_PD_شاخص_بټونه));

ساكن فراغ do_smmu_enable(فراغ)
{
    متطاير uint32_t *config = (متطاير uint32_t *)(MC_PADDR + SMMU_CONFIG_دننه_واټن);
    *config = 1;
}

ساكن فراغ do_smmu_disable(فراغ)
{
    متطاير uint32_t *config = (متطاير uint32_t *)(MC_PADDR + SMMU_CONFIG_دننه_واټن);
    *config = 0;
}

ساكن مضمن فراغ smmu_disable(فراغ)
{
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* in hyp mode, we need call the hook in monitor mode */
        /* we need physical address here */
        paddr_t addr = addrFromKPPtr(&do_smmu_disable);
        asm(".arch_extension sec\n");
        asm متطاير("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } وإلا {
        /* in secure mode, can enable it directly */
        smmu_regs->smmu_config = 0;
    }

    أرجع;
}

ساكن مضمن فراغ smmu_enable(فراغ)
{
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        paddr_t addr = addrFromKPPtr(&do_smmu_enable);
        asm(".arch_extension sec\n");
        asm متطاير("mov r0, %0\n\t"
                     "dsb\nisb\n"
                     "smc #0\n"
                     ::"r"(addr):"r0", "r1", "r2", "r3", "ip");
    } وإلا {
        smmu_regs->smmu_config = 1;
    }

    أرجع;
}


ساكن uint32_t make_ptb_معلومات(uint32_t pd_بنسټ, bool_t اقرأ, bool_t اكتب, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_بنسټ >> PTB_معلومات_بنسټ_ښويدنه);

    إذا (اقرأ) {
        ret |= PTB_معلومات_READ;
    }
    إذا (اكتب) {
        ret |= PTB_معلومات_WRITE;
    }
    إذا (nonsecure) {
        ret |= PTB_معلومات_NONSECURE;
    }

    أرجع ret;
}

فراغ plat_smmu_ptc_flush_all(فراغ)
{
    uint32_t cmd = PTC_FLUSH_ALL;
    smmu_regs->smmu_ptc_flush = cmd;
}

فراغ plat_smmu_tlb_flush_all(فراغ)
{
    uint32_t cmd = TLB_FLUSH_ALL;
    smmu_regs->smmu_tlb_flush = cmd;
}

BOOT_کوډ صحيح plat_smmu_init(فراغ)
{
    uint32_t asid;

    smmu_disable();

    لكل (asid = SMMU_لومړی_ASID; asid <= SMMU_وروستی_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_لومړی_ASID];

        memset(pd, 0, بټ(SMMU_PD_شاخص_بټونه));
        cleanCacheRange_RAM((کلمه_t)pd, ((کلمه_t)pd + بټ(SMMU_PD_شاخص_بټونه)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_معلومات = make_ptb_معلومات(pptr_منزل_paddr(pd), true, true, true);
    }
    چاپ_کړه("Total %d IOASID set up\n", (asid - 1));

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
    إذا (config_سېټ(CONFIG_SMMU_مداخله_ENABLE)) {
        smmu_regs->intmask = بټ(MC_APB_ASID_UPDATE_بټ) | بټ(MC_SMMU_پاڼه_بټ) |
                             بټ(MC_DECERR_MTS_بټ) | بټ(MC_SECERR_SEC_بټ) |
                             بټ(MC_DECERR_VPR_بټ) | بټ(MC_ARBITRATION_EMEM_بټ) |
                             بټ(MC_SECURITY_بټ) | بټ(MC_DECERR_EMEM_بټ);
    } وإلا {
        smmu_regs->intmask = 0;
    }
    أرجع ARM_PLAT_عدد_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    أكد(asid >= SMMU_لومړی_ASID && asid <= SMMU_وروستی_ASID);
    أرجع (iopde_t *) smmu_pds[asid - SMMU_لومړی_ASID];
}

فراغ plat_smmu_handle_مداخله(فراغ)
{
    uint32_t حالت = smmu_regs->intstatus;
    uint32_t clear_حالت = 0;

    إذا (حالت & بټ(MC_DECERR_MTS_بټ)) {
        clear_حالت |= بټ(MC_DECERR_MTS_بټ);
    }
    إذا (حالت & بټ(MC_SECERR_SEC_بټ)) {
        clear_حالت |= بټ(MC_SECERR_SEC_بټ);
    }
    إذا (حالت & بټ(MC_DECERR_VPR_بټ)) {
        clear_حالت |= بټ(MC_DECERR_VPR_بټ);
    }
    إذا (حالت & بټ(MC_ARBITRATION_EMEM_بټ)) {
        clear_حالت |= بټ(MC_ARBITRATION_EMEM_بټ);
    }
    إذا (حالت & بټ(MC_SECURITY_بټ)) {
        clear_حالت |= بټ(MC_SECURITY_بټ);
    }
    إذا (حالت & بټ(MC_DECERR_EMEM_بټ)) {
        clear_حالت |= بټ(MC_DECERR_EMEM_بټ);
    }
    إذا (حالت & بټ(MC_APB_ASID_UPDATE_بټ)) {
        clear_حالت |= بټ(MC_APB_ASID_UPDATE_بټ);
    }

    /* we only care about SMMU translation failures */
    إذا (حالت & بټ(MC_SMMU_پاڼه_بټ)) {
        إذا (config_سېټ(CONFIG_PRINTING)) {
            uint32_t err_حالت = smmu_regs->err_حالت;
            uint32_t ناکارول_شوی err_adr = smmu_regs->err_adr;
            uint32_t ناکارول_شوی id = err_حالت & MC_ERR_ID_ماسک;
            uint32_t ناکارول_شوی rw = (err_حالت & MC_ERR_RW_ماسک);
            uint32_t ناکارول_شوی اقرأ = (err_حالت & MC_ERR_INVALID_SMMU_پاڼه_READ_ماسک);
            uint32_t ناکارول_شوی اكتب = (err_حالت & MC_ERR_INVALID_SMMU_پاڼه_WRITE_ماسک);
            uint32_t ناکارول_شوی nonsecure = (err_حالت & MC_ERR_INVALID_SMMU_پاڼه_NONSECURE_ماسک);
            uint32_t ناکارول_شوی type = (err_حالت & MC_ERR_TYPE_ماسک) >> MC_ERR_TYPE_ښويدنه;

            چاپ_کړه("SMMU Address translation error:\n");
            چاپ_کړه("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            چاپ_کړه("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", اقرأ, اكتب, nonsecure);
        }
        clear_حالت |= بټ(MC_SMMU_پاڼه_بټ);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_حالت;
}
#د_که_پای
