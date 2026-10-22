/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <config.h>

#اگر_تعریف_شده CONFIG_TK1_SMMU

#ضمن <plat/machine/smmu.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <object/structures.h>


#تعریف_کن MC_PADDR                    0x70019000
#تعریف_کن SMMU_CONFIG_جابه_جایی          0x10

#تعریف_کن PTB_داده_پایه_لغزش         12
#تعریف_کن PTB_داده_READ               بیت(31)
#تعریف_کن PTB_داده_WRITE              بیت(30)
#تعریف_کن PTB_داده_NONSECURE          بیت(29)
#تعریف_کن PTB_داده_پایه_PD_ماسک       0x3fffff

#تعریف_کن MODULE_ASID_ENABLE          بیت(31)

#تعریف_کن PTC_FLUSH_ALL               0
#تعریف_کن PTC_FLUSH_ADR               1

#تعریف_کن TLB_ASID_تطابق              بیت(31)
#تعریف_کن TLB_FLUSH_ALL               (0)
#تعریف_کن TLB_FLUSH_SECTION           (2)
#تعریف_کن TLB_FLUSH_گروه             (3)

#تعریف_کن MC_DECERR_MTS_بیت           16u
#تعریف_کن MC_SECERR_SEC_بیت           13u
#تعریف_کن MC_DECERR_VPR_بیت           12u
#تعریف_کن MC_APB_ASID_UPDATE_بیت      11u
#تعریف_کن MC_SMMU_صفحه_بیت            10u
#تعریف_کن MC_ARBITRATION_EMEM_بیت     9u
#تعریف_کن MC_SECURITY_بیت             8u
#تعریف_کن MC_DECERR_EMEM_بیت          6u


#تعریف_کن MC_ERR_ID_ماسک              0x7f
#تعریف_کن MC_ERR_ADR_ماسک             0x7000
#تعریف_کن MC_ERR_RW_ماسک              0x10000
#تعریف_کن MC_ERR_SEC_ماسک             0x20000
#تعریف_کن MC_ERR_جابجا_ماسک            0x40000
#تعریف_کن MC_ERR_ADR_HI_ماسک          0x300000
#تعریف_کن MC_ERR_INVALID_SMMU_صفحه_NONSECURE_ماسک     0x2000000
#تعریف_کن MC_ERR_INVALID_SMMU_صفحه_WRITE_ماسک         0x4000000
#تعریف_کن MC_ERR_INVALID_SMMU_صفحه_READ_ماسک          0x8000000
#تعریف_کن MC_ERR_TYPE_ماسک                            0x70000000
#تعریف_کن MC_ERR_TYPE_لغزش                           28

#تعریف_کن MC_ERR_TYPE_RSVD                0
#تعریف_کن MC_ERR_TYPE_DECERR_EMEM         2
#تعریف_کن MC_ERR_TYPE_SECURITY            3
#تعریف_کن MC_ERR_TYPE_SECURITY_CARVEOUT   4
#تعریف_کن MC_ERR_TYPE_INVALID_SMMU_صفحه   6

#تعریف_کن IOPDE_4_M_شاخص_لغزش            22

ساكن متطاير tk1_mc_regs_t *smmu_regs = (متطاير tk1_mc_regs_t *)(SMMU_PPTR);

ساكن حرف smmu_pds[ARM_PLAT_رقم_SMMU][بیت(SMMU_PD_شاخص_بیتها)] ALIGN(بیت(SMMU_PD_شاخص_بیتها));

ساكن فراغ do_smmu_enable(فراغ)
{
    متطاير uint32_t *config = (متطاير uint32_t *)(MC_PADDR + SMMU_CONFIG_جابه_جایی);
    *config = 1;
}

ساكن فراغ do_smmu_disable(فراغ)
{
    متطاير uint32_t *config = (متطاير uint32_t *)(MC_PADDR + SMMU_CONFIG_جابه_جایی);
    *config = 0;
}

ساكن مضمن فراغ smmu_disable(فراغ)
{
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
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
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
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


ساكن uint32_t make_ptb_داده(uint32_t pd_پایه, bool_t اقرأ, bool_t اكتب, bool_t nonsecure)
{
    uint32_t ret = 0;
    ret = (pd_پایه >> PTB_داده_پایه_لغزش);

    إذا (اقرأ) {
        ret |= PTB_داده_READ;
    }
    إذا (اكتب) {
        ret |= PTB_داده_WRITE;
    }
    إذا (nonsecure) {
        ret |= PTB_داده_NONSECURE;
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

BOOT_کد صحيح plat_smmu_init(فراغ)
{
    uint32_t asid;

    smmu_disable();

    لكل (asid = SMMU_اولی_ASID; asid <= SMMU_آخرین_ASID; asid++) {
        iopde_t *pd = (iopde_t *) smmu_pds[asid - SMMU_اولی_ASID];

        memset(pd, 0, بیت(SMMU_PD_شاخص_بیتها));
        cleanCacheRange_RAM((کلمه_t)pd, ((کلمه_t)pd + بیت(SMMU_PD_شاخص_بیتها)),
                            addrFromPPtr(pd));

        smmu_regs->smmu_ptb_asid = asid;

        /* make it read/write/nonsecure but all translation entries are invalid */
        smmu_regs->smmu_ptb_داده = make_ptb_داده(pptr_مقصد_paddr(pd), true, true, true);
    }
    چاپ_کن("Total %d IOASID set up\n", (asid - 1));

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
    إذا (config_مجموعه(CONFIG_SMMU_وقفه_ENABLE)) {
        smmu_regs->intmask = بیت(MC_APB_ASID_UPDATE_بیت) | بیت(MC_SMMU_صفحه_بیت) |
                             بیت(MC_DECERR_MTS_بیت) | بیت(MC_SECERR_SEC_بیت) |
                             بیت(MC_DECERR_VPR_بیت) | بیت(MC_ARBITRATION_EMEM_بیت) |
                             بیت(MC_SECURITY_بیت) | بیت(MC_DECERR_EMEM_بیت);
    } وإلا {
        smmu_regs->intmask = 0;
    }
    أرجع ARM_PLAT_رقم_SMMU;
}

iopde_t *plat_smmu_lookup_iopd_by_asid(uint32_t asid)
{
    /* There should be no way to generate bad ASID values through the kernel
     * so this is an assertion and not a check */
    أكد(asid >= SMMU_اولی_ASID && asid <= SMMU_آخرین_ASID);
    أرجع (iopde_t *) smmu_pds[asid - SMMU_اولی_ASID];
}

فراغ plat_smmu_handle_وقفه(فراغ)
{
    uint32_t وضعیت = smmu_regs->intstatus;
    uint32_t clear_وضعیت = 0;

    إذا (وضعیت & بیت(MC_DECERR_MTS_بیت)) {
        clear_وضعیت |= بیت(MC_DECERR_MTS_بیت);
    }
    إذا (وضعیت & بیت(MC_SECERR_SEC_بیت)) {
        clear_وضعیت |= بیت(MC_SECERR_SEC_بیت);
    }
    إذا (وضعیت & بیت(MC_DECERR_VPR_بیت)) {
        clear_وضعیت |= بیت(MC_DECERR_VPR_بیت);
    }
    إذا (وضعیت & بیت(MC_ARBITRATION_EMEM_بیت)) {
        clear_وضعیت |= بیت(MC_ARBITRATION_EMEM_بیت);
    }
    إذا (وضعیت & بیت(MC_SECURITY_بیت)) {
        clear_وضعیت |= بیت(MC_SECURITY_بیت);
    }
    إذا (وضعیت & بیت(MC_DECERR_EMEM_بیت)) {
        clear_وضعیت |= بیت(MC_DECERR_EMEM_بیت);
    }
    إذا (وضعیت & بیت(MC_APB_ASID_UPDATE_بیت)) {
        clear_وضعیت |= بیت(MC_APB_ASID_UPDATE_بیت);
    }

    /* we only care about SMMU translation failures */
    إذا (وضعیت & بیت(MC_SMMU_صفحه_بیت)) {
        إذا (config_مجموعه(CONFIG_PRINTING)) {
            uint32_t err_وضعیت = smmu_regs->err_وضعیت;
            uint32_t استفادهنشده err_adr = smmu_regs->err_adr;
            uint32_t استفادهنشده id = err_وضعیت & MC_ERR_ID_ماسک;
            uint32_t استفادهنشده rw = (err_وضعیت & MC_ERR_RW_ماسک);
            uint32_t استفادهنشده اقرأ = (err_وضعیت & MC_ERR_INVALID_SMMU_صفحه_READ_ماسک);
            uint32_t استفادهنشده اكتب = (err_وضعیت & MC_ERR_INVALID_SMMU_صفحه_WRITE_ماسک);
            uint32_t استفادهنشده nonsecure = (err_وضعیت & MC_ERR_INVALID_SMMU_صفحه_NONSECURE_ماسک);
            uint32_t استفادهنشده type = (err_وضعیت & MC_ERR_TYPE_ماسک) >> MC_ERR_TYPE_لغزش;

            چاپ_کن("SMMU Address translation error:\n");
            چاپ_کن("ID: %d address: 0x%x type: %d direction: 0x%x\n", id, err_adr, type, rw);
            چاپ_کن("IOPT permission: read 0x%x write 0x%x nonsecure 0x%x\n", اقرأ, اكتب, nonsecure);
        }
        clear_وضعیت |= بیت(MC_SMMU_صفحه_بیت);
    }

    /* write 1 to clear the interrupt */
    smmu_regs->intstatus = clear_وضعیت;
}
#پایان_اگر
