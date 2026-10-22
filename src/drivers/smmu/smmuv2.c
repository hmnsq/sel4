/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_ARM_SMMU

#अन्तर्भाव <types.h>
#अन्तर्भाव <plat/machine/devices_gen.h>
#अन्तर्भाव <drivers/smmu/smmuv2.h>

/*supported stages of translations*/
#परिभाषय STAGE1_TRANS           (1 << 0)
#परिभाषय STAGE2_TRANS           (1 << 1)
#परिभाषय NESTED_TRANS           (1 << 2)
/*supported translation table formats*/
#परिभाषय AARCH32S_FMT           (1 << 0)
#परिभाषय AARCH32L_FMT           (1 << 1)
#परिभाषय NO_AARCH32_FMT         (1 << 2)
#परिभाषय TRANS_पृष्ठानि_4_KB        (1 << 3)
#परिभाषय TRANS_पृष्ठानि_16_KB       (1 << 4)
#परिभाषय TRANS_पृष्ठानि_64_KB       (1 << 5)

/*the default virtual address bits for partition TTBR0 and TTBR1*/
#परिभाषय SMMU_VA_DEFAULT_द्वयङ्काः      48

संरचना  smmu_feature {
    bool_t प्रवाहः_मेलनम्;              /*stream match register functionality included*/
    bool_t trans_op;                  /*address translation operations supported*/
    bool_t cotable_walk;              /*coherent translation table walk*/
    bool_t प्रसारणम्_tlb;             /*broadcast TLB maintenance*/
    bool_t vmid16;                    /*16 bits VMIDs are supported*/
    uint32_t supported_trans;         /*supported translation stages*/
    uint32_t supported_fmt;           /*supported translation formats*/
    uint32_t अङ्कः_cfault_ints;         /*supported number of context fault interrupts*/
    uint32_t अङ्कः_प्रवाहः_ids;          /*number of stream IDs*/
    uint32_t अङ्कः_प्रवाहः_map_groups;   /*num stream mapping register groups*/
    uint32_t smmu_page_आकार;          /*page size in SMMU register address space*/
    uint32_t smmu_अङ्कः_पृष्ठानि;          /*number of pages in global or context bank address space*/
    uint32_t अङ्कः_s2_cbanks;           /*cbanks that support stage 2 only*/
    uint32_t अङ्कः_cbanks;              /*total number of context banks*/
    uint32_t va_द्वयङ्काः;                 /*upstream address size*/
    uint32_t pa_द्वयङ्काः;                 /*PA address size*/
    uint32_t ipa_द्वयङ्काः;                /*IPA address size*/
    pptr_t cb_आधारः;                   /*base of context bank address space*/
};

संरचना smmu_सारणी_config {
    uint32_t tcr[2];                    /*SMMU_CBn_TCRm*/
    uint32_t mair[2];                  /*SMMU_CBn_MAIRm*/
    uint64_t ttbr[2];                  /*SMMU_CBn_TTBRm*/
};

स्थिर संरचना smmu_feature smmu_dev_knowledge;
स्थिर संरचना smmu_सारणी_config smmu_stage_सारणी_config;


स्थिर अन्तःस्थ uint32_t smmu_read_reg32(pptr_t आधारः, uint32_t सूचकांक)
{
    प्रत्यागम *(चञ्चल uint32_t *)(आधारः + सूचकांक);
}

स्थिर अन्तःस्थ शून्य smmu_write_reg32(pptr_t आधारः, uint32_t सूचकांक, uint32_t val)
{
    *(चञ्चल uint32_t *)(आधारः + सूचकांक) = val;
}

स्थिर अन्तःस्थ uint64_t smmu_read_reg64(pptr_t आधारः, uint32_t सूचकांक)
{
    प्रत्यागम *(चञ्चल uint64_t *)(आधारः + सूचकांक);
}

स्थिर अन्तःस्थ शून्य smmu_write_reg64(pptr_t आधारः, uint32_t सूचकांक, uint64_t val)
{
    *(चञ्चल uint64_t *)(आधारः + सूचकांक) = val;
}

स्थिर शून्य smmu_tlb_sync(pptr_t आधारः, uint32_t sync, uint32_t स्थितिः)
{
    पूर्णाङ्क गणनम् = 0;
    smmu_write_reg32(आधारः, sync, SMMU_TLB_SYNC_आवरणम्);
    यावत् (गणनम् < TLBSYNC_LOOP) {
        /*pulling the active flag, reading the TLB command state.*/
        यदि (!(smmu_read_reg32(आधारः, स्थितिः) & TLBSTATUS_GSACTIVE)) {
            भङ्ग;
        }
        गणनम्++;
    }
}

स्थिर अन्तःस्थ uint32_t smmu_obs_आकार_गन्तव्यम्_द्वयङ्काः(uint32_t आकार)
{
    /*converting the output bus address size into address bit, defined in
    IDx registers*/
    विकल्प (आकार) {
    पक्ष 0:
        प्रत्यागम 32;
    पक्ष 1:
        प्रत्यागम 36;
    पक्ष 2:
        प्रत्यागम 40;
    पक्ष 3:
        प्रत्यागम 42;
    पक्ष 4:
        प्रत्यागम 44;
    स्वभाव:
        प्रत्यागम 48;
    }
}
स्थिर अन्तःस्थ uint32_t smmu_ubs_आकार_गन्तव्यम्_द्वयङ्काः(uint32_t आकार)
{
    /*converting the upstream address size into address bit, defined in
    IDx registers*/
    विकल्प (आकार) {
    पक्ष 0:
        प्रत्यागम 32;
    पक्ष 1:
        प्रत्यागम 36;
    पक्ष 2:
        प्रत्यागम 40;
    पक्ष 3:
        प्रत्यागम 42;
    पक्ष 4:
        प्रत्यागम 44;
    पक्ष 5:
        प्रत्यागम 49;
    स्वभाव:
        प्रत्यागम 64;
    }
}

BOOT_संहिता स्थिर शून्य smmu_mapping_init(शून्य)
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
    map_kernel_frame(SMMU_GR1_PADDR(smmu_dev_knowledge.smmu_page_आकार),
                     SMMU_GR1_PPTR,
                     VMKernelOnly,
                     vm_गुणाः_new(true, false, false));
    /*GID registers*/
    map_kernel_frame(SMMU_GID_PADDR(smmu_dev_knowledge.smmu_page_आकार),
                     SMMU_GID_PPTR,
                     VMKernelOnly,
                     vm_गुणाः_new(true, false, false));
    /*PM registers*/
    map_kernel_frame(SMMU_PM_PADDR(smmu_dev_knowledge.smmu_page_आकार),
                     SMMU_PM_PPTR,
                     VMKernelOnly,
                     vm_गुणाः_new(true, false, false));
    /*SSD registers*/
    map_kernel_frame(SMMU_SSD_PADDR(smmu_dev_knowledge.smmu_page_आकार),
                     SMMU_SSD_PPTR,
                     VMKernelOnly,
                     vm_गुणाः_new(true, false, false));
    /*map the context banks, each bank maps to a 4K page*/
    प्रति (पूर्णाङ्क i = 0; i < smmu_dev_knowledge.अङ्कः_cbanks; i++) {
        map_kernel_frame(SMMU_CBn_PADDR(smmu_dev_knowledge.cb_आधारः, i, smmu_dev_knowledge.smmu_page_आकार),
                         SMMU_CBn_आधारः_PPTR(i),
                         VMKernelOnly,
                         vm_गुणाः_new(true, false, false));
    }
}

BOOT_संहिता स्थिर शून्य smmu_config_prob(शून्य)
{
    uint32_t reg, क्षेत्रम्;
    /*ID0*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR0);
    /*stages supported*/
    यदि (reg & IDR0_S1TS) {
        smmu_dev_knowledge.supported_trans |= STAGE1_TRANS;
    }
    यदि (reg & IDR0_S2TS) {
        smmu_dev_knowledge.supported_trans |= STAGE2_TRANS;
    }
    यदि (reg & IDR0_NTS) {
        smmu_dev_knowledge.supported_trans |= NESTED_TRANS;
    }
    /*stream matching register*/
    यदि (reg & IDR0_SMS) {
        smmu_dev_knowledge.प्रवाहः_मेलनम् = true;
    }
    /*address translation operation*/
    यदि ((reg & IDR0_ATOSNS) == 0) {
        smmu_dev_knowledge.trans_op = true;
    }
    /*AARCH32 translation format support*/
    क्षेत्रम् = IDR0_PTFS_VAL(reg & IDR0_PTFS);
    यदि (क्षेत्रम् == PTFS_AARCH32S_AARCH32L) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
        smmu_dev_knowledge.supported_fmt |= AARCH32S_FMT;
    } अन्यथा यदि (क्षेत्रम् == PTFS_AARCH32L_ONLY) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
    } अन्यथा {
        smmu_dev_knowledge.supported_fmt |= NO_AARCH32_FMT;
    }
    /*number of context fault interrupts
    * However, in smmuv2, each context bank has dedicated interrupt pin
    * hence no requirement to specify implemented interrupts here.*/
    smmu_dev_knowledge.अङ्कः_cfault_ints = IDR0_NUMIRPT_VAL(reg & IDR0_NUMIRPT);
    /*coherent translation table walk*/
    यदि (reg & IDR0_CTTW) {
        smmu_dev_knowledge.cotable_walk = true;
    }
    /*broadcast TLB maintenance*/
    यदि (reg & IDR0_BTM) {
        smmu_dev_knowledge.प्रसारणम्_tlb = true;
    }
    /*number of stream IDs*/
    smmu_dev_knowledge.अङ्कः_प्रवाहः_ids = (1 << IDR0_NUMSIDB_VAL(reg & IDR0_NUMSIDB)) - 1;
    /*number of stream mapping register groups*/
    smmu_dev_knowledge.अङ्कः_प्रवाहः_map_groups = reg & IDR0_NUMSMRG;

    /*ID1*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR1);
    /*smmu page size*/
    यदि (reg & IDR1_PAGESIZE) {
        smmu_dev_knowledge.smmu_page_आकार = SMMU_PAGE_64KB;
    } अन्यथा {
        smmu_dev_knowledge.smmu_page_आकार = SMMU_PAGE_4KB;
    }
    /*smmu num pages, 2^(numdxb + 1)*/
    क्षेत्रम् = IDR1_NUMPAGENDXB_VAL(reg & IDR1_NUMPAGENDXB);
    smmu_dev_knowledge.smmu_अङ्कः_पृष्ठानि = 1 << (क्षेत्रम् + 1);
    /*num of stage 2 context banks*/
    smmu_dev_knowledge.अङ्कः_s2_cbanks = IDR1_NUMS2CB_VAL(reg & IDR1_NUMS2CB);
    /*total num of context banks*/
    smmu_dev_knowledge.अङ्कः_cbanks = reg & IDR1_NUMCB;
    /*calculate the context bank base*/
    smmu_dev_knowledge.cb_आधारः = SMMU_CB_आधारः_PADDR(
                                     SMMU_GLOBAL_आकार(smmu_dev_knowledge.smmu_अङ्कः_पृष्ठानि, smmu_dev_knowledge.smmu_page_आकार));

    /*ID2*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR2);
    /*VNID16S*/
    यदि (reg & IDR2_VMID16S) {
        smmu_dev_knowledge.vmid16 = true;
    }
    /*PTFSV8_64KB*/
    यदि (reg & IDR2_PTFSV8_64) {
        smmu_dev_knowledge.supported_fmt |= TRANS_पृष्ठानि_64_KB;
    }
    /*PTFSV8_16KB*/
    यदि (reg & IDR2_PTFSV8_16) {
        smmu_dev_knowledge.supported_fmt |= TRANS_पृष्ठानि_16_KB;
    }
    /*PTFSV8_64KB*/

    यदि (reg & IDR2_PTFSV8_4) {
        smmu_dev_knowledge.supported_fmt |= TRANS_पृष्ठानि_4_KB;
    }
    /*UBS virtual address size*/
    smmu_dev_knowledge.va_द्वयङ्काः = smmu_ubs_आकार_गन्तव्यम्_द्वयङ्काः(IDR2_UBS_VAL(reg & IDR2_UBS));
    /*OAS*/
    smmu_dev_knowledge.pa_द्वयङ्काः = smmu_obs_आकार_गन्तव्यम्_द्वयङ्काः(IDR2_OAS_VAL(reg & IDR2_OAS));
    /*IAS*/
    smmu_dev_knowledge.ipa_द्वयङ्काः = smmu_obs_आकार_गन्तव्यम्_द्वयङ्काः(reg & IDR2_IAS);
}



BOOT_संहिता  स्थिर शून्य smmu_dev_reset(शून्य)
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
    reg = S2_CR_PRIVCFG_समुच्चयः(S2CR_PRIVCFG_DEFAULT);
    reg |= S2_CR_TYPE_समुच्चयः(S2CR_TYPE_CB);

    /*the number of stream-to-context is related to the stream indexing method*/
    यदि (smmu_dev_knowledge.प्रवाहः_मेलनम्) {
        /*stream matching*/
        प्रति (पूर्णाङ्क i = 0; i < smmu_dev_knowledge.अङ्कः_प्रवाहः_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(i), reg);
        }
        /*reset the stream match registers as invalid*/
        reg = SMR_VALID_समुच्चयः(SMR_VALID_DIS);
        प्रति (पूर्णाङ्क i = 0; i < smmu_dev_knowledge.अङ्कः_प्रवाहः_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(i), reg);
        }
    } अन्यथा {
        /*stream ID*/
        प्रति (पूर्णाङ्क i = 0; i < smmu_dev_knowledge.अङ्कः_प्रवाहः_ids; i++) {
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
    यदि (major >= 2) {
        reg &= ~ACR_CACHE_तालकम्;
    }
    /*enable the TLB to cache bypassing*/
    reg |= ACR_S2CRB_TLBEN | ACR_SMTNMB_TLBEN;
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sACR, reg);
    /*special init requested by the smmu-500: end*/

    प्रति (पूर्णाङ्क i = 0; i < smmu_dev_knowledge.अङ्कः_cbanks; i++) {
        cb_bank_ptr = SMMU_CBn_आधारः_PPTR(i);
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
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_आवरणम्);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_आवरणम्);

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

BOOT_संहिता शून्य plat_smmu_init(शून्य)
{
    smmu_config_prob();
    smmu_mapping_init();
    smmu_dev_reset();
}

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
स्थिर शून्य smmu_config_stage2(संरचना smmu_सारणी_config *cfg,
                               vspace_मूलम्_t *vspace)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    reg |= CBn_TCR_SH0_समुच्चयः(CBn_TCR_SH_INNER);
    reg |= CBn_TCR_ORGN0_समुच्चयः(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_IRGN0_समुच्चयः(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_TG0_समुच्चयः(CBn_TCR_TG_4K);
    /*setting according to the vcpu_init_vtcr in vcpu.h*/
#यदि_परिभाषितम् CONFIG_ARM_PA_आकार_द्वयङ्काः_40
    reg |= CBn_TCR_T0_SZ_समुच्चयः(24);
    reg |= CBn_TCR_PASize_समुच्चयः(CBn_TCR2_PASize_40);
    reg |= CBn_TCR_SL0_समुच्चयः(CBn_TCR_SL0_4KB_L1);
#अन्यथा
    reg |= CBn_TCR_T0_SZ_समुच्चयः(20);
    reg |= CBn_TCR_PASize_समुच्चयः(CBn_TCR2_PASize_44);
    reg |= CBn_TCR_SL0_समुच्चयः(CBn_TCR_SL0_4KB_L0);
#यदि_अन्तः
    /*reserved as 1*/
    reg |= BIT(31);
    cfg->tcr[0] = reg;
    /*vttbr*/
    cfg->ttbr[0] = ttbr_new(0, pptr_गन्तव्यम्_paddr(vspace)).शब्दाः[0];
}
#अन्यथा
स्थिर शून्य smmu_config_stage1(संरचना smmu_सारणी_config *cfg,
                               bool_t coherence, uint32_t pa_द्वयङ्काः,
                               vspace_मूलम्_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    यदि (coherence) {
        reg |= CBn_TCR_SH0_समुच्चयः(CBn_TCR_SH_INNER);
        reg |= CBn_TCR_ORGN0_समुच्चयः(CBn_TCR_GN_WB_WA_CACHE);
        reg |= CBn_TCR_IRGN0_समुच्चयः(CBn_TCR_GN_WB_WA_CACHE);
    } अन्यथा {
        reg |= CBn_TCR_SH0_समुच्चयः(CBn_TCR_SH_OUTER);
        reg |= CBn_TCR_ORGN0_समुच्चयः(CBn_TCR_GN_NCACHE);
        reg |= CBn_TCR_IRGN0_समुच्चयः(CBn_TCR_GN_NCACHE);
    }
    /*page size is configured as 4k*/
    reg |= CBn_TCR_TG0_समुच्चयः(CBn_TCR_TG_4K);
    /*the TTBR0 size, calculated according to the aarch64 formula*/
    reg |= CBn_TCR_T0_SZ_समुच्चयः(64 - SMMU_VA_DEFAULT_द्वयङ्काः);
    /*disable (speculative) page table walks through TTBR1*/
    reg |= CBn_TCR_EPD1_DIS;
    cfg->tcr[0] = reg;
    /*TCR2*/
    reg = 0;
    विकल्प (pa_द्वयङ्काः) {
    पक्ष 32:
        reg |= CBn_TCR2_PASize_समुच्चयः(CBn_TCR2_PASize_32);
        भङ्ग;
    पक्ष 36:
        reg |= CBn_TCR2_PASize_समुच्चयः(CBn_TCR2_PASize_36);
        भङ्ग;
    पक्ष 40:
        reg |= CBn_TCR2_PASize_समुच्चयः(CBn_TCR2_PASize_40);
        भङ्ग;
    पक्ष 42:
        reg |= CBn_TCR2_PASize_समुच्चयः(CBn_TCR2_PASize_42);
        भङ्ग;
    पक्ष 44:
        reg |= CBn_TCR2_PASize_समुच्चयः(CBn_TCR2_PASize_44);
        भङ्ग;
    स्वभाव:
        reg |= CBn_TCR2_PASize_समुच्चयः(CBn_TCR2_PASize_48);
        भङ्ग;
    }
    /*currently only support AArch64*/
    reg |= CBn_TCR2_SEP_समुच्चयः(CBn_TCR2_SEP_UPSTREAM_आकार) | CBn_TCR2_AS_समुच्चयः(CBn_TCR2_AS_16);
    cfg->tcr[1] = reg;
    /*MAIR0, configured according to the MAIR values in cores*/
    reg = CBn_MAIRm_ATTR_DEVICE_nGnRnE << CBn_MAIRm_ATTR_सरणम्(CBn_MAIRm_ATTR_ID_DEVICE_nGnRnE);
    reg |= CBn_MAIRm_ATTR_DEVICE_nGnRE << CBn_MAIRm_ATTR_सरणम्(CBn_MAIRm_ATTR_ID_DEVICE_nGnRE);
    reg |= CBn_MAIRm_ATTR_DEVICE_GRE << CBn_MAIRm_ATTR_सरणम्(CBn_MAIRm_ATTR_ID_DEVICE_GRE);
    reg |= CBn_MAIRm_ATTR_NC << CBn_MAIRm_ATTR_सरणम्(CBn_MAIRm_ATTR_ID_NC);
    cfg->mair[0] = reg;
    /*MAIR1*/
    reg = CBn_MAIRm_ATTR_CACHE << CBn_MAIRm_ATTR_सरणम्(CBn_MAIRm_ATTR_ID_CACHE);
    cfg->mair[1] = reg;
    /*TTBRs*/
    /*The SMMU only uses user-level address space, TTBR0.*/
    cfg->ttbr[0] = ttbr_new(asid, pptr_गन्तव्यम्_paddr(vspace)).शब्दाः[0];
    cfg->ttbr[1] = 0;
}
#यदि_अन्तः /*CONFIG_ARM_HYPERVISOR_SUPPORT*/


शून्य smmu_cb_नियोजय_vspace(word_t cb, vspace_मूलम्_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    uint32_t vmid = cb;
    /* For the stage 2 translation, the VMID space is designed as a private
     * space, its value is equal to the context bank index. Using private VMID
     * space avoids synchronising with vspace management on VMID reallocations.
     * Also, VMID used by SMMU need to be valid all time once device transactions
     * are enabled. To maintain the TLB coherency, we introduces a set of mechanism
      * that connects vspace to context banks linked via ASID. */
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    smmu_config_stage2(&smmu_stage_सारणी_config,
                       vspace);
#अन्यथा
    smmu_config_stage1(&smmu_stage_सारणी_config,
                       smmu_dev_knowledge.cotable_walk,
                       smmu_dev_knowledge.ipa_द्वयङ्काः,
                       vspace,
                       asid);
#यदि_अन्तः /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*CBA2R*/
    /*currently only support aarch64*/
    reg = CBA2_Rn_VA64_समुच्चयः;
    यदि (smmu_dev_knowledge.vmid16) {
        reg |= CBA2_Rn_VMID_समुच्चयः(vmid);
    }
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBA2Rn(cb), reg);

    /*CBAR*/
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2 translation only, CBAR_TYPE_S2_TRANS*/
    reg = CBARn_TYPE_समुच्चयः(CBARn_TYPE_STAGE2);
    /*8 bit VMID*/
    यदि (!smmu_dev_knowledge.vmid16) {
        reg |= CBARn_VMID_समुच्चयः(vmid);
    }
#अन्यथा
    /*stage 1 translation only, CBAR_TYPE_S1_TRANS_S2_BYPASS*/
    reg = CBARn_TYPE_समुच्चयः(CBARn_TYPE_STAGE1);
    /*configured as the weakest shareability/memory types,
     * so they can be overwritten by ttbcr or pte */
    reg |= CBARn_BPSHCFG_समुच्चयः(CBARn_BPSHCFG_NONE);
    reg |= CBARn_Mem_Attr_समुच्चयः(MemAttr_OWB_IWB);
#यदि_अन्तः  /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBARn(cb), reg);
    /*TCR*/
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TCR, smmu_stage_सारणी_config.tcr[0]);
    /* stage 1 translation requires both ttbr 1 and ttbr 0
     * stage 2 translation requires ttbr 0*/
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /*TCR2 is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TCR2, smmu_stage_सारणी_config.tcr[1]);
    smmu_write_reg64(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TTBR1, smmu_stage_सारणी_config.ttbr[1]);
#यदि_अन्तः /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/

    /*ttbr0 (user space), for both stage 1 and stage 2*/
    smmu_write_reg64(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TTBR0, smmu_stage_सारणी_config.ttbr[0]);
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /*MAIRs is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_MAIR0, smmu_stage_सारणी_config.mair[0]);
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_MAIR1, smmu_stage_सारणी_config.mair[1]);
#यदि_अन्तः /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*SCTLR, */
    reg = CBn_SCTLR_CFIE | CBn_SCTLR_CFRE | CBn_SCTLR_AFE | CBn_SCTLR_TRE | CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_SCTLR, reg);
}

शून्य smmu_cb_disable(word_t cb, asid_t asid)
{

    uint32_t reg = smmu_read_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_SCTLR);
    reg &= ~CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_SCTLR, reg);
    smmu_tlb_invalidate_cb(cb, asid);
}

शून्य smmu_sid_bind_cb(word_t sid, word_t cb)
{

    uint32_t reg = 0;
    reg = S2_CR_PRIVCFG_समुच्चयः(S2CR_PRIVCFG_DEFAULT);
    reg |= S2_CR_TYPE_समुच्चयः(S2CR_TYPE_CB);
    reg |= S2_CR_CBNDX_समुच्चयः(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    /* The number of stream-to-context mapping
     * is related to the stream indexing method.
     * We currently supports mapping one stream ID to one context bank.*/
    यदि (smmu_dev_knowledge.प्रवाहः_मेलनम्) {
        reg = SMR_VALID_समुच्चयः(SMR_VALID_EN) | SMR_ID_समुच्चयः(sid);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

शून्य smmu_sid_unbind(word_t sid)
{

    uint32_t reg =  S2_CR_TYPE_समुच्चयः(S2CR_TYPE_FAULT);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    यदि (smmu_dev_knowledge.प्रवाहः_मेलनम्) {
        reg = SMR_VALID_समुच्चयः(SMR_VALID_DIS);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

शून्य smmu_tlb_invalidate_all(शून्य)
{
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /*on hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_आवरणम्);
#अन्यथा
    /*on non-secure non-hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_आवरणम्);
#यदि_अन्तः
    /*syn above TLB operations*/
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
}

शून्य smmu_tlb_invalidate_cb(पूर्णाङ्क cb, asid_t asid)
{
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* SMMU uses a private VMID space. Each context bank assigns its VMID with its
     * context bnak number.*/
    uint32_t reg = TLBIVMID_समुच्चयः(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIVMID, reg);
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
#अन्यथा
    /*stage 1*/
    uint32_t reg = CBn_TLBIASID_समुच्चयः(asid);
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TLBIASID, reg);
    smmu_tlb_sync(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#यदि_अन्तः
}

शून्य smmu_tlb_invalidate_cb_va(पूर्णाङ्क cb, asid_t asid, vptr_t vaddr)
{
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* invalidate all unlocated TLB entries in the stage 2 translation
    * associated with the given IPA*/
    uint64_t reg = CBn_TLBIIPAS2_समुच्चयः(vaddr);
    smmu_write_reg64(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TLBIIPAS2, reg);
    smmu_tlb_sync(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#अन्यथा
    /*stage 1*/
    uint64_t reg = CBn_TLBIVA_समुच्चयः(asid, vaddr);
    smmu_write_reg64(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TLBIVA, reg);
    smmu_tlb_sync(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#यदि_अन्तः
}

शून्य smmu_read_fault_state(uint32_t *स्थितिः, uint32_t *syndrome_0, uint32_t *syndrome_1)
{
    *स्थितिः = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    *syndrome_0 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR0);
    *syndrome_1 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR1);
}

शून्य smmu_clear_fault_state(शून्य)
{
    uint32_t reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSR, reg);
}

शून्य smmu_cb_read_fault_state(पूर्णाङ्क cb, uint32_t *स्थितिः, word_t *स्थानाङ्कः)
{
    *स्थितिः = smmu_read_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_FSR);
    *स्थानाङ्कः = smmu_read_reg64(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_FAR);
}

शून्य smmu_cb_clear_fault_state(पूर्णाङ्क cb)
{
    smmu_write_reg32(SMMU_CBn_आधारः_PPTR(cb), SMMU_CBn_FSR, CBn_FSR_CLEAR_ALL);
}

#यदि_अन्तः /* CONFIG_ARM_SMMU */
