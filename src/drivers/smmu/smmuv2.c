/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#اگر_تعریف_شده CONFIG_ARM_SMMU

#ضمن <types.h>
#ضمن <plat/machine/devices_gen.h>
#ضمن <drivers/smmu/smmuv2.h>

/*supported stages of translations*/
#تعریف_کن STAGE1_TRANS           (1 << 0)
#تعریف_کن STAGE2_TRANS           (1 << 1)
#تعریف_کن NESTED_TRANS           (1 << 2)
/*supported translation table formats*/
#تعریف_کن AARCH32S_FMT           (1 << 0)
#تعریف_کن AARCH32L_FMT           (1 << 1)
#تعریف_کن NO_AARCH32_FMT         (1 << 2)
#تعریف_کن TRANS_صفحات_4_KB        (1 << 3)
#تعریف_کن TRANS_صفحات_16_KB       (1 << 4)
#تعریف_کن TRANS_صفحات_64_KB       (1 << 5)

/*the default virtual address bits for partition TTBR0 and TTBR1*/
#تعریف_کن SMMU_VA_DEFAULT_بیتها      48

بنية  smmu_feature {
    bool_t جریان_تطابق;              /*stream match register functionality included*/
    bool_t trans_op;                  /*address translation operations supported*/
    bool_t cotable_walk;              /*coherent translation table walk*/
    bool_t پخش_tlb;             /*broadcast TLB maintenance*/
    bool_t vmid16;                    /*16 bits VMIDs are supported*/
    uint32_t supported_trans;         /*supported translation stages*/
    uint32_t supported_fmt;           /*supported translation formats*/
    uint32_t رقم_cfault_ints;         /*supported number of context fault interrupts*/
    uint32_t رقم_جریان_ids;          /*number of stream IDs*/
    uint32_t رقم_جریان_map_groups;   /*num stream mapping register groups*/
    uint32_t smmu_صفحه_اندازه;          /*page size in SMMU register address space*/
    uint32_t smmu_رقم_صفحات;          /*number of pages in global or context bank address space*/
    uint32_t رقم_s2_cbanks;           /*cbanks that support stage 2 only*/
    uint32_t رقم_cbanks;              /*total number of context banks*/
    uint32_t va_بیتها;                 /*upstream address size*/
    uint32_t pa_بیتها;                 /*PA address size*/
    uint32_t ipa_بیتها;                /*IPA address size*/
    pptr_t cb_پایه;                   /*base of context bank address space*/
};

بنية smmu_جدول_config {
    uint32_t tcr[2];                    /*SMMU_CBn_TCRm*/
    uint32_t mair[2];                  /*SMMU_CBn_MAIRm*/
    uint64_t ttbr[2];                  /*SMMU_CBn_TTBRm*/
};

ساكن بنية smmu_feature smmu_dev_knowledge;
ساكن بنية smmu_جدول_config smmu_stage_جدول_config;


ساكن مضمن uint32_t smmu_read_reg32(pptr_t پایه, uint32_t شاخص)
{
    أرجع *(متطاير uint32_t *)(پایه + شاخص);
}

ساكن مضمن فراغ smmu_write_reg32(pptr_t پایه, uint32_t شاخص, uint32_t val)
{
    *(متطاير uint32_t *)(پایه + شاخص) = val;
}

ساكن مضمن uint64_t smmu_read_reg64(pptr_t پایه, uint32_t شاخص)
{
    أرجع *(متطاير uint64_t *)(پایه + شاخص);
}

ساكن مضمن فراغ smmu_write_reg64(pptr_t پایه, uint32_t شاخص, uint64_t val)
{
    *(متطاير uint64_t *)(پایه + شاخص) = val;
}

ساكن فراغ smmu_tlb_sync(pptr_t پایه, uint32_t sync, uint32_t وضعیت)
{
    صحيح شمار = 0;
    smmu_write_reg32(پایه, sync, SMMU_TLB_SYNC_ماسک);
    طالما (شمار < TLBSYNC_LOOP) {
        /*pulling the active flag, reading the TLB command state.*/
        إذا (!(smmu_read_reg32(پایه, وضعیت) & TLBSTATUS_GSACTIVE)) {
            اكسر;
        }
        شمار++;
    }
}

ساكن مضمن uint32_t smmu_obs_اندازه_مقصد_بیتها(uint32_t اندازه)
{
    /*converting the output bus address size into address bit, defined in
    IDx registers*/
    بدل (اندازه) {
    حالة 0:
        أرجع 32;
    حالة 1:
        أرجع 36;
    حالة 2:
        أرجع 40;
    حالة 3:
        أرجع 42;
    حالة 4:
        أرجع 44;
    افتراضي:
        أرجع 48;
    }
}
ساكن مضمن uint32_t smmu_ubs_اندازه_مقصد_بیتها(uint32_t اندازه)
{
    /*converting the upstream address size into address bit, defined in
    IDx registers*/
    بدل (اندازه) {
    حالة 0:
        أرجع 32;
    حالة 1:
        أرجع 36;
    حالة 2:
        أرجع 40;
    حالة 3:
        أرجع 42;
    حالة 4:
        أرجع 44;
    حالة 5:
        أرجع 49;
    افتراضي:
        أرجع 64;
    }
}

BOOT_کد ساكن فراغ smmu_mapping_init(فراغ)
{
    /*Creating mapping for the rest of SMMU address space.
     * the code assumes registers in each SMMU page are located in a 4K page
     * even though the alignment of the (physical) pages can be 64K.
     * We make this assumption to compact the SMMU virtual address window.*/

    /* This is a temporary solution. A correct solution should be adjust
     * the virtual address space layout of the kernel, leaving enough virtual
     * address space to SMMU windows. For example, SMMU on TX2 requires a 8M space
     * in total, including those empty areas resulted from the 64K alignment.
     * Also, kernel requires device space to be configured statically. To
     * support populate device space using HW config, we need to modify
     * kernel_frame_t and map_kernel_frame, allowing devices mapped in a
     * separate page table using HW config.*/

    /*the current implementation has been only tested on the TX2 platform*/

    /*init the GR1 region, start: smmu_pptr + 4K, size 4K*/
    map_kernel_قاب(SMMU_GR1_PADDR(smmu_dev_knowledge.smmu_صفحه_اندازه),
                     SMMU_GR1_PPTR,
                     VMKernelOnly,
                     vm_صفات_new(true, false, false));
    /*GID registers*/
    map_kernel_قاب(SMMU_GID_PADDR(smmu_dev_knowledge.smmu_صفحه_اندازه),
                     SMMU_GID_PPTR,
                     VMKernelOnly,
                     vm_صفات_new(true, false, false));
    /*PM registers*/
    map_kernel_قاب(SMMU_PM_PADDR(smmu_dev_knowledge.smmu_صفحه_اندازه),
                     SMMU_PM_PPTR,
                     VMKernelOnly,
                     vm_صفات_new(true, false, false));
    /*SSD registers*/
    map_kernel_قاب(SMMU_SSD_PADDR(smmu_dev_knowledge.smmu_صفحه_اندازه),
                     SMMU_SSD_PPTR,
                     VMKernelOnly,
                     vm_صفات_new(true, false, false));
    /*map the context banks, each bank maps to a 4K page*/
    لكل (صحيح i = 0; i < smmu_dev_knowledge.رقم_cbanks; i++) {
        map_kernel_قاب(SMMU_CBn_PADDR(smmu_dev_knowledge.cb_پایه, i, smmu_dev_knowledge.smmu_صفحه_اندازه),
                         SMMU_CBn_پایه_PPTR(i),
                         VMKernelOnly,
                         vm_صفات_new(true, false, false));
    }
}

BOOT_کد ساكن فراغ smmu_config_prob(فراغ)
{
    uint32_t reg, فیلد;
    /*ID0*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR0);
    /*stages supported*/
    إذا (reg & IDR0_S1TS) {
        smmu_dev_knowledge.supported_trans |= STAGE1_TRANS;
    }
    إذا (reg & IDR0_S2TS) {
        smmu_dev_knowledge.supported_trans |= STAGE2_TRANS;
    }
    إذا (reg & IDR0_NTS) {
        smmu_dev_knowledge.supported_trans |= NESTED_TRANS;
    }
    /*stream matching register*/
    إذا (reg & IDR0_SMS) {
        smmu_dev_knowledge.جریان_تطابق = true;
    }
    /*address translation operation*/
    إذا ((reg & IDR0_ATOSNS) == 0) {
        smmu_dev_knowledge.trans_op = true;
    }
    /*AARCH32 translation format support*/
    فیلد = IDR0_PTFS_VAL(reg & IDR0_PTFS);
    إذا (فیلد == PTFS_AARCH32S_AARCH32L) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
        smmu_dev_knowledge.supported_fmt |= AARCH32S_FMT;
    } وإلا إذا (فیلد == PTFS_AARCH32L_ONLY) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
    } وإلا {
        smmu_dev_knowledge.supported_fmt |= NO_AARCH32_FMT;
    }
    /*number of context fault interrupts
    * However, in smmuv2, each context bank has dedicated interrupt pin
    * hence no requirement to specify implemented interrupts here.*/
    smmu_dev_knowledge.رقم_cfault_ints = IDR0_NUMIRPT_VAL(reg & IDR0_NUMIRPT);
    /*coherent translation table walk*/
    إذا (reg & IDR0_CTTW) {
        smmu_dev_knowledge.cotable_walk = true;
    }
    /*broadcast TLB maintenance*/
    إذا (reg & IDR0_BTM) {
        smmu_dev_knowledge.پخش_tlb = true;
    }
    /*number of stream IDs*/
    smmu_dev_knowledge.رقم_جریان_ids = (1 << IDR0_NUMSIDB_VAL(reg & IDR0_NUMSIDB)) - 1;
    /*number of stream mapping register groups*/
    smmu_dev_knowledge.رقم_جریان_map_groups = reg & IDR0_NUMSMRG;

    /*ID1*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR1);
    /*smmu page size*/
    إذا (reg & IDR1_PAGESIZE) {
        smmu_dev_knowledge.smmu_صفحه_اندازه = SMMU_صفحه_64_KB;
    } وإلا {
        smmu_dev_knowledge.smmu_صفحه_اندازه = SMMU_صفحه_4_KB;
    }
    /*smmu num pages, 2^(numdxb + 1)*/
    فیلد = IDR1_NUMPAGENDXB_VAL(reg & IDR1_NUMPAGENDXB);
    smmu_dev_knowledge.smmu_رقم_صفحات = 1 << (فیلد + 1);
    /*num of stage 2 context banks*/
    smmu_dev_knowledge.رقم_s2_cbanks = IDR1_NUMS2CB_VAL(reg & IDR1_NUMS2CB);
    /*total num of context banks*/
    smmu_dev_knowledge.رقم_cbanks = reg & IDR1_NUMCB;
    /*calculate the context bank base*/
    smmu_dev_knowledge.cb_پایه = SMMU_CB_پایه_PADDR(
                                     SMMU_GLOBAL_اندازه(smmu_dev_knowledge.smmu_رقم_صفحات, smmu_dev_knowledge.smmu_صفحه_اندازه));

    /*ID2*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR2);
    /*VNID16S*/
    إذا (reg & IDR2_VMID16S) {
        smmu_dev_knowledge.vmid16 = true;
    }
    /*PTFSV8_64KB*/
    إذا (reg & IDR2_PTFSV8_64) {
        smmu_dev_knowledge.supported_fmt |= TRANS_صفحات_64_KB;
    }
    /*PTFSV8_16KB*/
    إذا (reg & IDR2_PTFSV8_16) {
        smmu_dev_knowledge.supported_fmt |= TRANS_صفحات_16_KB;
    }
    /*PTFSV8_64KB*/

    إذا (reg & IDR2_PTFSV8_4) {
        smmu_dev_knowledge.supported_fmt |= TRANS_صفحات_4_KB;
    }
    /*UBS virtual address size*/
    smmu_dev_knowledge.va_بیتها = smmu_ubs_اندازه_مقصد_بیتها(IDR2_UBS_VAL(reg & IDR2_UBS));
    /*OAS*/
    smmu_dev_knowledge.pa_بیتها = smmu_obs_اندازه_مقصد_بیتها(IDR2_OAS_VAL(reg & IDR2_OAS));
    /*IAS*/
    smmu_dev_knowledge.ipa_بیتها = smmu_obs_اندازه_مقصد_بیتها(reg & IDR2_IAS);
}



BOOT_کد  ساكن فراغ smmu_dev_reset(فراغ)
{
    uint32_t reg = 0;
    pptr_t cb_bank_ptr;
    uint32_t major;

    /*clear the fault syndrome registers*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR0, reg);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR1, reg);
    /*clear the global FSR by writing back the read value*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSR, reg);

    /*reset stream to context config as using context banks*/
    reg = S2_CR_PRIVCFG_مجموعه(S2CR_PRIVCFG_DEFAULT);
    reg |= S2_CR_TYPE_مجموعه(S2CR_TYPE_CB);

    /*the number of stream-to-context is related to the stream indexing method*/
    إذا (smmu_dev_knowledge.جریان_تطابق) {
        /*stream matching*/
        لكل (صحيح i = 0; i < smmu_dev_knowledge.رقم_جریان_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(i), reg);
        }
        /*reset the stream match registers as invalid*/
        reg = SMR_VALID_مجموعه(SMR_VALID_DIS);
        لكل (صحيح i = 0; i < smmu_dev_knowledge.رقم_جریان_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(i), reg);
        }
    } وإلا {
        /*stream ID*/
        لكل (صحيح i = 0; i < smmu_dev_knowledge.رقم_جریان_ids; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(i), reg);
        }
    }

    /*special init requested by the smmu-500: start*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR7);
    major = IDR7_MAJOR_VAL(reg & IDR7_MAJOR);
    /*init the auxiliary configuration register*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sACR);
    /*unlock the write access to SMMU_CBn_ACTLR,
    only provided in version 2 and above*/
    إذا (major >= 2) {
        reg &= ~ACR_CACHE_قفل;
    }
    /*enable the TLB to cache bypassing*/
    reg |= ACR_S2CRB_TLBEN | ACR_SMTNMB_TLBEN;
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sACR, reg);
    /*special init requested by the smmu-500: end*/

    لكل (صحيح i = 0; i < smmu_dev_knowledge.رقم_cbanks; i++) {
        cb_bank_ptr = SMMU_CBn_پایه_PPTR(i);
        /*disable context banks and clear the context bank fault registers*/
        smmu_write_reg32(cb_bank_ptr, SMMU_CBn_SCTLR, 0);
        /*clear the syndrome register*/
        smmu_write_reg64(cb_bank_ptr, SMMU_CBn_FAR, 0ULL);
        smmu_write_reg32(cb_bank_ptr, SMMU_CBn_FSR, CBn_FSR_CLEAR_ALL);
        /*special init requested by the smmu-500: start*/
        /*disable MMU-500's next page prefetch due to errata 841119 and 826419*/
        reg = smmu_read_reg32(cb_bank_ptr, SMMU_CBn_ACTLR);
        reg &= ~CBn_ACTLR_CPRE;
        smmu_write_reg32(cb_bank_ptr, SMMU_CBn_ACTLR, reg);
        /*special init requested by the smmu-500: end*/
    }

    /*invalidate TLB */
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_ماسک);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_ماسک);

    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sCR0);
    /*enable global fault reporting*/
    reg |= CR0_GFRE | CR0_GFIE | CR0_GCFGFRE | CR0_GCFGFIE;
    /*raise fault for any transaction that does not match to
    any stream mapping table entries*/
    reg |= CR0_USFCFG;
    /*raise fault for stream match conflict*/
    reg |= CR0_SMCFCFG;
    /*enable the VMID private name space*/
    reg |= CR0_VMIDPNE;
    /*TLB is maintained together with the rest of the system*/
    reg &= ~CR0_PTM;
    /*enable force TLB broadcast on bypassing transactions*/
    reg |= CR0_FB;
    /*enable client access, ie transaction enforced by SMMU*/
    reg &= ~CR0_CLIENTPD;
    /*upgrade barrier to full system*/
    reg &= ~CR0_BSU(CR0_BSU_ALL);
    /*syn above issued TLB operations*/
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
    /*enable the SMMU*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sCR0, reg);
}

BOOT_کد فراغ plat_smmu_init(فراغ)
{
    smmu_config_prob();
    smmu_mapping_init();
    smmu_dev_reset();
}

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن فراغ smmu_config_stage2(بنية smmu_جدول_config *cfg,
                               vspace_ریشه_t *vspace)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    reg |= CBn_TCR_SH0_مجموعه(CBn_TCR_SH_INNER);
    reg |= CBn_TCR_ORGN0_مجموعه(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_IRGN0_مجموعه(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_TG0_مجموعه(CBn_TCR_TG_4K);
    /*setting according to the vcpu_init_vtcr in vcpu.h*/
#اگر_تعریف_شده CONFIG_ARM_PA_اندازه_بیتها_40
    reg |= CBn_TCR_T0_SZ_مجموعه(24);
    reg |= CBn_TCR_PASize_مجموعه(CBn_TCR2_PASize_40);
    reg |= CBn_TCR_SL0_مجموعه(CBn_TCR_SL0_4KB_L1);
#وإلا
    reg |= CBn_TCR_T0_SZ_مجموعه(20);
    reg |= CBn_TCR_PASize_مجموعه(CBn_TCR2_PASize_44);
    reg |= CBn_TCR_SL0_مجموعه(CBn_TCR_SL0_4KB_L0);
#پایان_اگر
    /*reserved as 1*/
    reg |= بیت(31);
    cfg->tcr[0] = reg;
    /*vttbr*/
    cfg->ttbr[0] = ttbr_new(0, pptr_مقصد_paddr(vspace)).واژگان[0];
}
#وإلا
ساكن فراغ smmu_config_stage1(بنية smmu_جدول_config *cfg,
                               bool_t coherence, uint32_t pa_بیتها,
                               vspace_ریشه_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    إذا (coherence) {
        reg |= CBn_TCR_SH0_مجموعه(CBn_TCR_SH_INNER);
        reg |= CBn_TCR_ORGN0_مجموعه(CBn_TCR_GN_WB_WA_CACHE);
        reg |= CBn_TCR_IRGN0_مجموعه(CBn_TCR_GN_WB_WA_CACHE);
    } وإلا {
        reg |= CBn_TCR_SH0_مجموعه(CBn_TCR_SH_OUTER);
        reg |= CBn_TCR_ORGN0_مجموعه(CBn_TCR_GN_NCACHE);
        reg |= CBn_TCR_IRGN0_مجموعه(CBn_TCR_GN_NCACHE);
    }
    /*page size is configured as 4k*/
    reg |= CBn_TCR_TG0_مجموعه(CBn_TCR_TG_4K);
    /*the TTBR0 size, calculated according to the aarch64 formula*/
    reg |= CBn_TCR_T0_SZ_مجموعه(64 - SMMU_VA_DEFAULT_بیتها);
    /*disable (speculative) page table walks through TTBR1*/
    reg |= CBn_TCR_EPD1_DIS;
    cfg->tcr[0] = reg;
    /*TCR2*/
    reg = 0;
    بدل (pa_بیتها) {
    حالة 32:
        reg |= CBn_TCR2_PASize_مجموعه(CBn_TCR2_PASize_32);
        اكسر;
    حالة 36:
        reg |= CBn_TCR2_PASize_مجموعه(CBn_TCR2_PASize_36);
        اكسر;
    حالة 40:
        reg |= CBn_TCR2_PASize_مجموعه(CBn_TCR2_PASize_40);
        اكسر;
    حالة 42:
        reg |= CBn_TCR2_PASize_مجموعه(CBn_TCR2_PASize_42);
        اكسر;
    حالة 44:
        reg |= CBn_TCR2_PASize_مجموعه(CBn_TCR2_PASize_44);
        اكسر;
    افتراضي:
        reg |= CBn_TCR2_PASize_مجموعه(CBn_TCR2_PASize_48);
        اكسر;
    }
    /*currently only support AArch64*/
    reg |= CBn_TCR2_SEP_مجموعه(CBn_TCR2_SEP_UPSTREAM_اندازه) | CBn_TCR2_AS_مجموعه(CBn_TCR2_AS_16);
    cfg->tcr[1] = reg;
    /*MAIR0, configured according to the MAIR values in cores*/
    reg = CBn_MAIRm_ATTR_DEVICE_nGnRnE << CBn_MAIRm_ATTR_لغزش(CBn_MAIRm_ATTR_ID_DEVICE_nGnRnE);
    reg |= CBn_MAIRm_ATTR_DEVICE_nGnRE << CBn_MAIRm_ATTR_لغزش(CBn_MAIRm_ATTR_ID_DEVICE_nGnRE);
    reg |= CBn_MAIRm_ATTR_DEVICE_GRE << CBn_MAIRm_ATTR_لغزش(CBn_MAIRm_ATTR_ID_DEVICE_GRE);
    reg |= CBn_MAIRm_ATTR_NC << CBn_MAIRm_ATTR_لغزش(CBn_MAIRm_ATTR_ID_NC);
    cfg->mair[0] = reg;
    /*MAIR1*/
    reg = CBn_MAIRm_ATTR_CACHE << CBn_MAIRm_ATTR_لغزش(CBn_MAIRm_ATTR_ID_CACHE);
    cfg->mair[1] = reg;
    /*TTBRs*/
    /*The SMMU only uses user-level address space, TTBR0.*/
    cfg->ttbr[0] = ttbr_new(asid, pptr_مقصد_paddr(vspace)).واژگان[0];
    cfg->ttbr[1] = 0;
}
#پایان_اگر /*CONFIG_ARM_HYPERVISOR_SUPPORT*/


فراغ smmu_cb_تخصیص_بده_vspace(کلمه_t cb, vspace_ریشه_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    uint32_t vmid = cb;
    /* For the stage 2 translation, the VMID space is designed as a private
     * space, its value is equal to the context bank index. Using private VMID
     * space avoids synchronising with vspace management on VMID reallocations.
     * Also, VMID used by SMMU need to be valid all time once device transactions
     * are enabled. To maintain the TLB coherency, we introduces a set of mechanism
      * that connects vspace to context banks linked via ASID. */
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    smmu_config_stage2(&smmu_stage_جدول_config,
                       vspace);
#وإلا
    smmu_config_stage1(&smmu_stage_جدول_config,
                       smmu_dev_knowledge.cotable_walk,
                       smmu_dev_knowledge.ipa_بیتها,
                       vspace,
                       asid);
#پایان_اگر /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*CBA2R*/
    /*currently only support aarch64*/
    reg = CBA2_Rn_VA64_مجموعه;
    إذا (smmu_dev_knowledge.vmid16) {
        reg |= CBA2_Rn_VMID_مجموعه(vmid);
    }
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBA2Rn(cb), reg);

    /*CBAR*/
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2 translation only, CBAR_TYPE_S2_TRANS*/
    reg = CBARn_TYPE_مجموعه(CBARn_TYPE_STAGE2);
    /*8 bit VMID*/
    إذا (!smmu_dev_knowledge.vmid16) {
        reg |= CBARn_VMID_مجموعه(vmid);
    }
#وإلا
    /*stage 1 translation only, CBAR_TYPE_S1_TRANS_S2_BYPASS*/
    reg = CBARn_TYPE_مجموعه(CBARn_TYPE_STAGE1);
    /*configured as the weakest shareability/memory types,
     * so they can be overwritten by ttbcr or pte */
    reg |= CBARn_BPSHCFG_مجموعه(CBARn_BPSHCFG_NONE);
    reg |= CBARn_Mem_Attr_مجموعه(MemAttr_OWB_IWB);
#پایان_اگر  /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBARn(cb), reg);
    /*TCR*/
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TCR, smmu_stage_جدول_config.tcr[0]);
    /* stage 1 translation requires both ttbr 1 and ttbr 0
     * stage 2 translation requires ttbr 0*/
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    /*TCR2 is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TCR2, smmu_stage_جدول_config.tcr[1]);
    smmu_write_reg64(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TTBR1, smmu_stage_جدول_config.ttbr[1]);
#پایان_اگر /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/

    /*ttbr0 (user space), for both stage 1 and stage 2*/
    smmu_write_reg64(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TTBR0, smmu_stage_جدول_config.ttbr[0]);
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    /*MAIRs is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_MAIR0, smmu_stage_جدول_config.mair[0]);
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_MAIR1, smmu_stage_جدول_config.mair[1]);
#پایان_اگر /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*SCTLR, */
    reg = CBn_SCTLR_CFIE | CBn_SCTLR_CFRE | CBn_SCTLR_AFE | CBn_SCTLR_TRE | CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_SCTLR, reg);
}

فراغ smmu_cb_disable(کلمه_t cb, asid_t asid)
{

    uint32_t reg = smmu_read_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_SCTLR);
    reg &= ~CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_SCTLR, reg);
    smmu_tlb_invalidate_cb(cb, asid);
}

فراغ smmu_sid_bind_cb(کلمه_t sid, کلمه_t cb)
{

    uint32_t reg = 0;
    reg = S2_CR_PRIVCFG_مجموعه(S2CR_PRIVCFG_DEFAULT);
    reg |= S2_CR_TYPE_مجموعه(S2CR_TYPE_CB);
    reg |= S2_CR_CBNDX_مجموعه(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    /* The number of stream-to-context mapping
     * is related to the stream indexing method.
     * We currently supports mapping one stream ID to one context bank.*/
    إذا (smmu_dev_knowledge.جریان_تطابق) {
        reg = SMR_VALID_مجموعه(SMR_VALID_EN) | SMR_ID_مجموعه(sid);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

فراغ smmu_sid_unbind(کلمه_t sid)
{

    uint32_t reg =  S2_CR_TYPE_مجموعه(S2CR_TYPE_FAULT);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    إذا (smmu_dev_knowledge.جریان_تطابق) {
        reg = SMR_VALID_مجموعه(SMR_VALID_DIS);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

فراغ smmu_tlb_invalidate_all(فراغ)
{
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    /*on hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_ماسک);
#وإلا
    /*on non-secure non-hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_ماسک);
#پایان_اگر
    /*syn above TLB operations*/
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
}

فراغ smmu_tlb_invalidate_cb(صحيح cb, asid_t asid)
{
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* SMMU uses a private VMID space. Each context bank assigns its VMID with its
     * context bnak number.*/
    uint32_t reg = TLBIVMID_مجموعه(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIVMID, reg);
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
#وإلا
    /*stage 1*/
    uint32_t reg = CBn_TLBIASID_مجموعه(asid);
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TLBIASID, reg);
    smmu_tlb_sync(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#پایان_اگر
}

فراغ smmu_tlb_invalidate_cb_va(صحيح cb, asid_t asid, vptr_t vaddr)
{
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* invalidate all unlocated TLB entries in the stage 2 translation
    * associated with the given IPA*/
    uint64_t reg = CBn_TLBIIPAS2_مجموعه(vaddr);
    smmu_write_reg64(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TLBIIPAS2, reg);
    smmu_tlb_sync(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#وإلا
    /*stage 1*/
    uint64_t reg = CBn_TLBIVA_مجموعه(asid, vaddr);
    smmu_write_reg64(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TLBIVA, reg);
    smmu_tlb_sync(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#پایان_اگر
}

فراغ smmu_read_fault_state(uint32_t *وضعیت, uint32_t *syndrome_0, uint32_t *syndrome_1)
{
    *وضعیت = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    *syndrome_0 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR0);
    *syndrome_1 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR1);
}

فراغ smmu_clear_fault_state(فراغ)
{
    uint32_t reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSR, reg);
}

فراغ smmu_cb_read_fault_state(صحيح cb, uint32_t *وضعیت, کلمه_t *آدرس)
{
    *وضعیت = smmu_read_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_FSR);
    *آدرس = smmu_read_reg64(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_FAR);
}

فراغ smmu_cb_clear_fault_state(صحيح cb)
{
    smmu_write_reg32(SMMU_CBn_پایه_PPTR(cb), SMMU_CBn_FSR, CBn_FSR_CLEAR_ALL);
}

#پایان_اگر /* CONFIG_ARM_SMMU */
