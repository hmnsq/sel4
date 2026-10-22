/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_TK1_SMMU

#ضمن <plat/machine/smmu.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <object/structures.h>


#تعریف_کرو MC_PADDR                    0x70019000
#تعریف_کرو SMMU_CONFIG_وتھ          0x10

#تعریف_کرو PTB_انکڑے_ادھار_کھسکاؤ         12
#تعریف_کرو PTB_انکڑے_READ               ثنائی_ہندسہ(31)
#تعریف_کرو PTB_انکڑے_WRITE              ثنائی_ہندسہ(30)
#تعریف_کرو PTB_انکڑے_NONSECURE          ثنائی_ہندسہ(29)
#تعریف_کرو PTB_انکڑے_ادھار_PD_نقاب       0x3fffff

#تعریف_کرو MODULE_ASID_ENABLE          ثنائی_ہندسہ(31)

#تعریف_کرو PTC_FLUSH_ALL               0
#تعریف_کرو PTC_FLUSH_ADR               1

#تعریف_کرو TLB_ASID_میل              ثنائی_ہندسہ(31)
#تعریف_کرو TLB_FLUSH_ALL               (0)
#تعریف_کرو TLB_FLUSH_SECTION           (2)
#تعریف_کرو TLB_FLUSH_گروہ             (3)

#تعریف_کرو MC_DECERR_MTS_ثنائی_ہندسہ           16u
#تعریف_کرو MC_SECERR_SEC_ثنائی_ہندسہ           13u
#تعریف_کرو MC_DECERR_VPR_ثنائی_ہندسہ           12u
#تعریف_کرو MC_APB_ASID_UPDATE_ثنائی_ہندسہ      11u
#تعریف_کرو MC_SMMU_ورقہ_ثنائی_ہندسہ            10u
#تعریف_کرو MC_ARBITRATION_EMEM_ثنائی_ہندسہ     9u
#تعریف_کرو MC_SECURITY_ثنائی_ہندسہ             8u
#تعریف_کرو MC_DECERR_EMEM_ثنائی_ہندسہ          6u


#تعریف_کرو MC_ERR_ID_نقاب              0x7f
#تعریف_کرو MC_ERR_ADR_نقاب             0x7000
#تعریف_کرو MC_ERR_RW_نقاب              0x10000
#تعریف_کرو MC_ERR_SEC_نقاب             0x20000
#تعریف_کرو MC_ERR_وٹاندرا_نقاب            0x40000
#تعریف_کرو MC_ERR_ADR_HI_نقاب          0x300000
#تعریف_کرو MC_ERR_INVALID_SMMU_ورقہ_NONSECURE_نقاب     0x2000000
#تعریف_کرو MC_ERR_INVALID_SMMU_ورقہ_WRITE_نقاب         0x4000000
#تعریف_کرو MC_ERR_INVALID_SMMU_ورقہ_READ_نقاب          0x8000000
#تعریف_کرو MC_ERR_TYPE_نقاب                            0x70000000
#تعریف_کرو MC_ERR_TYPE_کھسکاؤ                           28

#تعریف_کرو MC_ERR_TYPE_RSVD                0
#تعریف_کرو MC_ERR_TYPE_DECERR_EMEM         2
#تعریف_کرو MC_ERR_TYPE_SECURITY            3
#تعریف_کرو MC_ERR_TYPE_SECURITY_CARVEOUT   4
#تعریف_کرو MC_ERR_TYPE_INVALID_SMMU_ورقہ   6

#تعریف_کرو IOPDE_4_M_اشاریہ_کھسکاؤ            22

ساكن متطاير tk1_mc_regs_t *smmu_regs = (متطاير tk1_mc_regs_t *)(SMMU_PPTR);

ساكن حرف smmu_pds[ARM_PLAT_عدد_SMMU][ثنائی_ہندسہ(SMMU_PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(SMMU_PD_اشاریہ_بٹ));

ساكن فراغ do_smmu_enable(فراغ)
{
    متطاير uint32_t *config = (متطاير uint32_t *)(MC_PADDR + SMMU_CONFIG_وتھ);
    *config = 1;
}

ساكن فراغ do_smmu_disable(فراغ)
{
    متطاير uint32_t *config = (متطاير uint32_t *)(MC_PADDR + SMMU_CONFIG_وتھ);
    *config = 0;
}

ساكن مضمن فراغ smmu_disable(فراغ)
{
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
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
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
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


ساكن uint32_t make_ptb_انکڑے(uint32_t pd_ادھار, bool_t اقرأ, bool_t اكتب, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_ادھار >> PTB_انکڑے_ادھار_کھسکاؤ);

    إذا (اقرأ) {
        ret |= PTB_انکڑے_READ;
    }
    إذا (اكتب) {
        ret |= PTB_انکڑے_WRITE;
    }
    إذا (nonsecure) {
        ret |= PTB_انکڑے_NONSECURE;
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

BOOT_کوڈ صحيح plat_smmu_init(فراغ)
{
    uint32_t asid;

    smmu_disable();

    لكل (asid = SMMU_پہلا_ASID; asid <= SMMU_آخری_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_پہلا_ASID];

        memset(pd, 0, ثنائی_ہندسہ(SMMU_PD_اشاریہ_بٹ));
        cleanCacheRange_RAM((لفظ_t)pd, ((لفظ_t)pd + ثنائی_ہندسہ(SMMU_PD_اشاریہ_بٹ)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_انکڑے = make_ptb_انکڑے(pptr_منزل_paddr(pd), true, true, true);
    }
    لکھو("Total %d IOASID set up\n", (asid - 1));

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
    إذا (config_سیٹ(CONFIG_SMMU_رکاوٹ_ENABLE)) {
        smmu_regs->intmask = ثنائی_ہندسہ(MC_APB_ASID_UPDATE_ثنائی_ہندسہ) | ثنائی_ہندسہ(MC_SMMU_ورقہ_ثنائی_ہندسہ) |
                             ثنائی_ہندسہ(MC_DECERR_MTS_ثنائی_ہندسہ) | ثنائی_ہندسہ(MC_SECERR_SEC_ثنائی_ہندسہ) |
                             ثنائی_ہندسہ(MC_DECERR_VPR_ثنائی_ہندسہ) | ثنائی_ہندسہ(MC_ARBITRATION_EMEM_ثنائی_ہندسہ) |
                             ثنائی_ہندسہ(MC_SECURITY_ثنائی_ہندسہ) | ثنائی_ہندسہ(MC_DECERR_EMEM_ثنائی_ہندسہ);
    } وإلا {
        smmu_regs->intmask = 0;
    }
    أرجع ARM_PLAT_عدد_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    أكد(asid >= SMMU_پہلا_ASID && asid <= SMMU_آخری_ASID);
    أرجع (iopde_t *) smmu_pds[asid - SMMU_پہلا_ASID];
}

فراغ plat_smmu_handle_رکاوٹ(فراغ)
{
    uint32_t حالت = smmu_regs->intstatus;
    uint32_t clear_حالت = 0;

    إذا (حالت & ثنائی_ہندسہ(MC_DECERR_MTS_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_DECERR_MTS_ثنائی_ہندسہ);
    }
    إذا (حالت & ثنائی_ہندسہ(MC_SECERR_SEC_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_SECERR_SEC_ثنائی_ہندسہ);
    }
    إذا (حالت & ثنائی_ہندسہ(MC_DECERR_VPR_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_DECERR_VPR_ثنائی_ہندسہ);
    }
    إذا (حالت & ثنائی_ہندسہ(MC_ARBITRATION_EMEM_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_ARBITRATION_EMEM_ثنائی_ہندسہ);
    }
    إذا (حالت & ثنائی_ہندسہ(MC_SECURITY_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_SECURITY_ثنائی_ہندسہ);
    }
    إذا (حالت & ثنائی_ہندسہ(MC_DECERR_EMEM_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_DECERR_EMEM_ثنائی_ہندسہ);
    }
    إذا (حالت & ثنائی_ہندسہ(MC_APB_ASID_UPDATE_ثنائی_ہندسہ)) {
        clear_حالت |= ثنائی_ہندسہ(MC_APB_ASID_UPDATE_ثنائی_ہندسہ);
    }

    /* we only care about SMMU translation failures */
    إذا (حالت & ثنائی_ہندسہ(MC_SMMU_ورقہ_ثنائی_ہندسہ)) {
        إذا (config_سیٹ(CONFIG_PRINTING)) {
            uint32_t err_حالت = smmu_regs->err_حالت;
            uint32_t بن_ورتیا err_adr = smmu_regs->err_adr;
            uint32_t بن_ورتیا id = err_حالت & MC_ERR_ID_نقاب;
            uint32_t بن_ورتیا rw = (err_حالت & MC_ERR_RW_نقاب);
            uint32_t بن_ورتیا اقرأ = (err_حالت & MC_ERR_INVALID_SMMU_ورقہ_READ_نقاب);
            uint32_t بن_ورتیا اكتب = (err_حالت & MC_ERR_INVALID_SMMU_ورقہ_WRITE_نقاب);
            uint32_t بن_ورتیا nonsecure = (err_حالت & MC_ERR_INVALID_SMMU_ورقہ_NONSECURE_نقاب);
            uint32_t بن_ورتیا type = (err_حالت & MC_ERR_TYPE_نقاب) >> MC_ERR_TYPE_کھسکاؤ;

            لکھو("SMMU Address translation error:\n");
            لکھو("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            لکھو("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", اقرأ, اكتب, nonsecure);
        }
        clear_حالت |= ثنائی_ہندسہ(MC_SMMU_ورقہ_ثنائی_ہندسہ);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_حالت;
}
#جے_انت
