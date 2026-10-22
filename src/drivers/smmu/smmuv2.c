/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <config.h>

#ifdef CONFIG_ARM_SMMU

#अन्तर्भाव <types.h>
#अन्तर्भाव <plat/machine/devices_gen.h>
#अन्तर्भाव <drivers/smmu/smmuv2.h>

/*supported stages of translations*/
#define STAGE1_TRANS           (1 << 0)
#define STAGE2_TRANS           (1 << 1)
#define NESTED_TRANS           (1 << 2)
/*supported translation table formats*/
#define AARCH32S_FMT           (1 << 0)
#define AARCH32L_FMT           (1 << 1)
#define NO_AARCH32_FMT         (1 << 2)
#define TRANS_PAGES_4KB        (1 << 3)
#define TRANS_PAGES_16KB       (1 << 4)
#define TRANS_PAGES_64KB       (1 << 5)

/*the default virtual address bits for partition TTBR0 and TTBR1*/
#define SMMU_VA_DEFAULT_BITS      48

संरचना  smmu_feature {
    bool_t stream_match;              /*stream match register functionality included*/
    bool_t trans_op;                  /*address translation operations supported*/
    bool_t cotable_walk;              /*coherent translation table walk*/
    bool_t broadcast_tlb;             /*broadcast TLB maintenance*/
    bool_t vmid16;                    /*16 bits VMIDs are supported*/
    uint32_t supported_trans;         /*supported translation stages*/
    uint32_t supported_fmt;           /*supported translation formats*/
    uint32_t अंक_cfault_ints;         /*supported number of context fault interrupts*/
    uint32_t अंक_stream_ids;          /*number of stream IDs*/
    uint32_t अंक_stream_map_groups;   /*num stream mapping register groups*/
    uint32_t smmu_page_size;          /*page size in SMMU register address space*/
    uint32_t smmu_अंक_pages;          /*number of pages in global or context bank address space*/
    uint32_t अंक_s2_cbanks;           /*cbanks that support stage 2 only*/
    uint32_t अंक_cbanks;              /*total number of context banks*/
    uint32_t va_bits;                 /*upstream address size*/
    uint32_t pa_bits;                 /*PA address size*/
    uint32_t ipa_bits;                /*IPA address size*/
    pptr_t cb_base;                   /*base of context bank address space*/
};

संरचना smmu_table_config {
    uint32_t tcr[2];                    /*SMMU_CBn_TCRm*/
    uint32_t mair[2];                  /*SMMU_CBn_MAIRm*/
    uint64_t ttbr[2];                  /*SMMU_CBn_TTBRm*/
};

स्थिर संरचना smmu_feature smmu_dev_knowledge;
स्थिर संरचना smmu_table_config smmu_stage_table_config;


स्थिर ओळीत uint32_t smmu_read_reg32(pptr_t base, uint32_t index)
{
    परत *(अस्थिर uint32_t *)(base + index);
}

स्थिर ओळीत शून्य smmu_write_reg32(pptr_t base, uint32_t index, uint32_t val)
{
    *(अस्थिर uint32_t *)(base + index) = val;
}

स्थिर ओळीत uint64_t smmu_read_reg64(pptr_t base, uint32_t index)
{
    परत *(अस्थिर uint64_t *)(base + index);
}

स्थिर ओळीत शून्य smmu_write_reg64(pptr_t base, uint32_t index, uint64_t val)
{
    *(अस्थिर uint64_t *)(base + index) = val;
}

स्थिर शून्य smmu_tlb_sync(pptr_t base, uint32_t sync, uint32_t status)
{
    पूर्णांक मोजणी = 0;
    smmu_write_reg32(base, sync, SMMU_TLB_SYNC_MASK);
    पर्यंत (मोजणी < TLBSYNC_LOOP) {
        /*pulling the active flag, reading the TLB command state.*/
        जर (!(smmu_read_reg32(base, status) & TLBSTATUS_GSACTIVE)) {
            थांबा;
        }
        मोजणी++;
    }
}

स्थिर ओळीत uint32_t smmu_obs_size_to_bits(uint32_t size)
{
    /*converting the output bus address size into address bit, defined in
    IDx registers*/
    निवड (size) {
    स्थिती 0:
        परत 32;
    स्थिती 1:
        परत 36;
    स्थिती 2:
        परत 40;
    स्थिती 3:
        परत 42;
    स्थिती 4:
        परत 44;
    पूर्वनिर्धारित:
        परत 48;
    }
}
स्थिर ओळीत uint32_t smmu_ubs_size_to_bits(uint32_t size)
{
    /*converting the upstream address size into address bit, defined in
    IDx registers*/
    निवड (size) {
    स्थिती 0:
        परत 32;
    स्थिती 1:
        परत 36;
    स्थिती 2:
        परत 40;
    स्थिती 3:
        परत 42;
    स्थिती 4:
        परत 44;
    स्थिती 5:
        परत 49;
    पूर्वनिर्धारित:
        परत 64;
    }
}

BOOT_CODE स्थिर शून्य smmu_mapping_init(शून्य)
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
    map_kernel_frame(SMMU_GR1_PADDR(smmu_dev_knowledge.smmu_page_size),
                     SMMU_GR1_PPTR,
                     VMKernelOnly,
                     vm_attributes_new(true, false, false));
    /*GID registers*/
    map_kernel_frame(SMMU_GID_PADDR(smmu_dev_knowledge.smmu_page_size),
                     SMMU_GID_PPTR,
                     VMKernelOnly,
                     vm_attributes_new(true, false, false));
    /*PM registers*/
    map_kernel_frame(SMMU_PM_PADDR(smmu_dev_knowledge.smmu_page_size),
                     SMMU_PM_PPTR,
                     VMKernelOnly,
                     vm_attributes_new(true, false, false));
    /*SSD registers*/
    map_kernel_frame(SMMU_SSD_PADDR(smmu_dev_knowledge.smmu_page_size),
                     SMMU_SSD_PPTR,
                     VMKernelOnly,
                     vm_attributes_new(true, false, false));
    /*map the context banks, each bank maps to a 4K page*/
    साठी (पूर्णांक i = 0; i < smmu_dev_knowledge.अंक_cbanks; i++) {
        map_kernel_frame(SMMU_CBn_PADDR(smmu_dev_knowledge.cb_base, i, smmu_dev_knowledge.smmu_page_size),
                         SMMU_CBn_BASE_PPTR(i),
                         VMKernelOnly,
                         vm_attributes_new(true, false, false));
    }
}

BOOT_CODE स्थिर शून्य smmu_config_prob(शून्य)
{
    uint32_t reg, field;
    /*ID0*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR0);
    /*stages supported*/
    जर (reg & IDR0_S1TS) {
        smmu_dev_knowledge.supported_trans |= STAGE1_TRANS;
    }
    जर (reg & IDR0_S2TS) {
        smmu_dev_knowledge.supported_trans |= STAGE2_TRANS;
    }
    जर (reg & IDR0_NTS) {
        smmu_dev_knowledge.supported_trans |= NESTED_TRANS;
    }
    /*stream matching register*/
    जर (reg & IDR0_SMS) {
        smmu_dev_knowledge.stream_match = true;
    }
    /*address translation operation*/
    जर ((reg & IDR0_ATOSNS) == 0) {
        smmu_dev_knowledge.trans_op = true;
    }
    /*AARCH32 translation format support*/
    field = IDR0_PTFS_VAL(reg & IDR0_PTFS);
    जर (field == PTFS_AARCH32S_AARCH32L) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
        smmu_dev_knowledge.supported_fmt |= AARCH32S_FMT;
    } अन्यथा जर (field == PTFS_AARCH32L_ONLY) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
    } अन्यथा {
        smmu_dev_knowledge.supported_fmt |= NO_AARCH32_FMT;
    }
    /*number of context fault interrupts
    * However, in smmuv2, each context bank has dedicated interrupt pin
    * hence no requirement to specify implemented interrupts here.*/
    smmu_dev_knowledge.अंक_cfault_ints = IDR0_NUMIRPT_VAL(reg & IDR0_NUMIRPT);
    /*coherent translation table walk*/
    जर (reg & IDR0_CTTW) {
        smmu_dev_knowledge.cotable_walk = true;
    }
    /*broadcast TLB maintenance*/
    जर (reg & IDR0_BTM) {
        smmu_dev_knowledge.broadcast_tlb = true;
    }
    /*number of stream IDs*/
    smmu_dev_knowledge.अंक_stream_ids = (1 << IDR0_NUMSIDB_VAL(reg & IDR0_NUMSIDB)) - 1;
    /*number of stream mapping register groups*/
    smmu_dev_knowledge.अंक_stream_map_groups = reg & IDR0_NUMSMRG;

    /*ID1*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR1);
    /*smmu page size*/
    जर (reg & IDR1_PAGESIZE) {
        smmu_dev_knowledge.smmu_page_size = SMMU_PAGE_64KB;
    } अन्यथा {
        smmu_dev_knowledge.smmu_page_size = SMMU_PAGE_4KB;
    }
    /*smmu num pages, 2^(numdxb + 1)*/
    field = IDR1_NUMPAGENDXB_VAL(reg & IDR1_NUMPAGENDXB);
    smmu_dev_knowledge.smmu_अंक_pages = 1 << (field + 1);
    /*num of stage 2 context banks*/
    smmu_dev_knowledge.अंक_s2_cbanks = IDR1_NUMS2CB_VAL(reg & IDR1_NUMS2CB);
    /*total num of context banks*/
    smmu_dev_knowledge.अंक_cbanks = reg & IDR1_NUMCB;
    /*calculate the context bank base*/
    smmu_dev_knowledge.cb_base = SMMU_CB_BASE_PADDR(
                                     SMMU_GLOBAL_SIZE(smmu_dev_knowledge.smmu_अंक_pages, smmu_dev_knowledge.smmu_page_size));

    /*ID2*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR2);
    /*VNID16S*/
    जर (reg & IDR2_VMID16S) {
        smmu_dev_knowledge.vmid16 = true;
    }
    /*PTFSV8_64KB*/
    जर (reg & IDR2_PTFSV8_64) {
        smmu_dev_knowledge.supported_fmt |= TRANS_PAGES_64KB;
    }
    /*PTFSV8_16KB*/
    जर (reg & IDR2_PTFSV8_16) {
        smmu_dev_knowledge.supported_fmt |= TRANS_PAGES_16KB;
    }
    /*PTFSV8_64KB*/

    जर (reg & IDR2_PTFSV8_4) {
        smmu_dev_knowledge.supported_fmt |= TRANS_PAGES_4KB;
    }
    /*UBS virtual address size*/
    smmu_dev_knowledge.va_bits = smmu_ubs_size_to_bits(IDR2_UBS_VAL(reg & IDR2_UBS));
    /*OAS*/
    smmu_dev_knowledge.pa_bits = smmu_obs_size_to_bits(IDR2_OAS_VAL(reg & IDR2_OAS));
    /*IAS*/
    smmu_dev_knowledge.ipa_bits = smmu_obs_size_to_bits(reg & IDR2_IAS);
}



BOOT_CODE  स्थिर शून्य smmu_dev_reset(शून्य)
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
    reg = S2CR_PRIVCFG_SET(S2CR_PRIVCFG_DEFAULT);
    reg |= S2CR_TYPE_SET(S2CR_TYPE_CB);

    /*the number of stream-to-context is related to the stream indexing method*/
    जर (smmu_dev_knowledge.stream_match) {
        /*stream matching*/
        साठी (पूर्णांक i = 0; i < smmu_dev_knowledge.अंक_stream_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(i), reg);
        }
        /*reset the stream match registers as invalid*/
        reg = SMR_VALID_SET(SMR_VALID_DIS);
        साठी (पूर्णांक i = 0; i < smmu_dev_knowledge.अंक_stream_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(i), reg);
        }
    } अन्यथा {
        /*stream ID*/
        साठी (पूर्णांक i = 0; i < smmu_dev_knowledge.अंक_stream_ids; i++) {
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
    जर (major >= 2) {
        reg &= ~ACR_CACHE_LOCK;
    }
    /*enable the TLB to cache bypassing*/
    reg |= ACR_S2CRB_TLBEN | ACR_SMTNMB_TLBEN;
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sACR, reg);
    /*special init requested by the smmu-500: end*/

    साठी (पूर्णांक i = 0; i < smmu_dev_knowledge.अंक_cbanks; i++) {
        cb_bank_ptr = SMMU_CBn_BASE_PPTR(i);
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
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_MASK);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_MASK);

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

BOOT_CODE शून्य plat_smmu_init(शून्य)
{
    smmu_config_prob();
    smmu_mapping_init();
    smmu_dev_reset();
}

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
स्थिर शून्य smmu_config_stage2(संरचना smmu_table_config *cfg,
                               vspace_मूळ_t *vspace)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    reg |= CBn_TCR_SH0_SET(CBn_TCR_SH_INNER);
    reg |= CBn_TCR_ORGN0_SET(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_IRGN0_SET(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_TG0_SET(CBn_TCR_TG_4K);
    /*setting according to the vcpu_init_vtcr in vcpu.h*/
#ifdef CONFIG_ARM_PA_SIZE_BITS_40
    reg |= CBn_TCR_T0SZ_SET(24);
    reg |= CBn_TCR_PASize_SET(CBn_TCR2_PASize_40);
    reg |= CBn_TCR_SL0_SET(CBn_TCR_SL0_4KB_L1);
#अन्यथा
    reg |= CBn_TCR_T0SZ_SET(20);
    reg |= CBn_TCR_PASize_SET(CBn_TCR2_PASize_44);
    reg |= CBn_TCR_SL0_SET(CBn_TCR_SL0_4KB_L0);
#endif
    /*reserved as 1*/
    reg |= BIT(31);
    cfg->tcr[0] = reg;
    /*vttbr*/
    cfg->ttbr[0] = ttbr_new(0, pptr_to_paddr(vspace)).शब्द[0];
}
#अन्यथा
स्थिर शून्य smmu_config_stage1(संरचना smmu_table_config *cfg,
                               bool_t coherence, uint32_t pa_bits,
                               vspace_मूळ_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    जर (coherence) {
        reg |= CBn_TCR_SH0_SET(CBn_TCR_SH_INNER);
        reg |= CBn_TCR_ORGN0_SET(CBn_TCR_GN_WB_WA_CACHE);
        reg |= CBn_TCR_IRGN0_SET(CBn_TCR_GN_WB_WA_CACHE);
    } अन्यथा {
        reg |= CBn_TCR_SH0_SET(CBn_TCR_SH_OUTER);
        reg |= CBn_TCR_ORGN0_SET(CBn_TCR_GN_NCACHE);
        reg |= CBn_TCR_IRGN0_SET(CBn_TCR_GN_NCACHE);
    }
    /*page size is configured as 4k*/
    reg |= CBn_TCR_TG0_SET(CBn_TCR_TG_4K);
    /*the TTBR0 size, calculated according to the aarch64 formula*/
    reg |= CBn_TCR_T0SZ_SET(64 - SMMU_VA_DEFAULT_BITS);
    /*disable (speculative) page table walks through TTBR1*/
    reg |= CBn_TCR_EPD1_DIS;
    cfg->tcr[0] = reg;
    /*TCR2*/
    reg = 0;
    निवड (pa_bits) {
    स्थिती 32:
        reg |= CBn_TCR2_PASize_SET(CBn_TCR2_PASize_32);
        थांबा;
    स्थिती 36:
        reg |= CBn_TCR2_PASize_SET(CBn_TCR2_PASize_36);
        थांबा;
    स्थिती 40:
        reg |= CBn_TCR2_PASize_SET(CBn_TCR2_PASize_40);
        थांबा;
    स्थिती 42:
        reg |= CBn_TCR2_PASize_SET(CBn_TCR2_PASize_42);
        थांबा;
    स्थिती 44:
        reg |= CBn_TCR2_PASize_SET(CBn_TCR2_PASize_44);
        थांबा;
    पूर्वनिर्धारित:
        reg |= CBn_TCR2_PASize_SET(CBn_TCR2_PASize_48);
        थांबा;
    }
    /*currently only support AArch64*/
    reg |= CBn_TCR2_SEP_SET(CBn_TCR2_SEP_UPSTREAM_SIZE) | CBn_TCR2_AS_SET(CBn_TCR2_AS_16);
    cfg->tcr[1] = reg;
    /*MAIR0, configured according to the MAIR values in cores*/
    reg = CBn_MAIRm_ATTR_DEVICE_nGnRnE << CBn_MAIRm_ATTR_SHIFT(CBn_MAIRm_ATTR_ID_DEVICE_nGnRnE);
    reg |= CBn_MAIRm_ATTR_DEVICE_nGnRE << CBn_MAIRm_ATTR_SHIFT(CBn_MAIRm_ATTR_ID_DEVICE_nGnRE);
    reg |= CBn_MAIRm_ATTR_DEVICE_GRE << CBn_MAIRm_ATTR_SHIFT(CBn_MAIRm_ATTR_ID_DEVICE_GRE);
    reg |= CBn_MAIRm_ATTR_NC << CBn_MAIRm_ATTR_SHIFT(CBn_MAIRm_ATTR_ID_NC);
    cfg->mair[0] = reg;
    /*MAIR1*/
    reg = CBn_MAIRm_ATTR_CACHE << CBn_MAIRm_ATTR_SHIFT(CBn_MAIRm_ATTR_ID_CACHE);
    cfg->mair[1] = reg;
    /*TTBRs*/
    /*The SMMU only uses user-level address space, TTBR0.*/
    cfg->ttbr[0] = ttbr_new(asid, pptr_to_paddr(vspace)).शब्द[0];
    cfg->ttbr[1] = 0;
}
#endif /*CONFIG_ARM_HYPERVISOR_SUPPORT*/


शून्य smmu_cb_assign_vspace(word_t cb, vspace_मूळ_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    uint32_t vmid = cb;
    /* For the stage 2 translation, the VMID space is designed as a private
     * space, its value is equal to the context bank index. Using private VMID
     * space avoids synchronising with vspace management on VMID reallocations.
     * Also, VMID used by SMMU need to be valid all time once device transactions
     * are enabled. To maintain the TLB coherency, we introduces a set of mechanism
      * that connects vspace to context banks linked via ASID. */
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    smmu_config_stage2(&smmu_stage_table_config,
                       vspace);
#अन्यथा
    smmu_config_stage1(&smmu_stage_table_config,
                       smmu_dev_knowledge.cotable_walk,
                       smmu_dev_knowledge.ipa_bits,
                       vspace,
                       asid);
#endif /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*CBA2R*/
    /*currently only support aarch64*/
    reg = CBA2Rn_VA64_SET;
    जर (smmu_dev_knowledge.vmid16) {
        reg |= CBA2Rn_VMID_SET(vmid);
    }
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBA2Rn(cb), reg);

    /*CBAR*/
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2 translation only, CBAR_TYPE_S2_TRANS*/
    reg = CBARn_TYPE_SET(CBARn_TYPE_STAGE2);
    /*8 bit VMID*/
    जर (!smmu_dev_knowledge.vmid16) {
        reg |= CBARn_VMID_SET(vmid);
    }
#अन्यथा
    /*stage 1 translation only, CBAR_TYPE_S1_TRANS_S2_BYPASS*/
    reg = CBARn_TYPE_SET(CBARn_TYPE_STAGE1);
    /*configured as the weakest shareability/memory types,
     * so they can be overwritten by ttbcr or pte */
    reg |= CBARn_BPSHCFG_SET(CBARn_BPSHCFG_NONE);
    reg |= CBARn_MemAttr_SET(MemAttr_OWB_IWB);
#endif  /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBARn(cb), reg);
    /*TCR*/
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TCR, smmu_stage_table_config.tcr[0]);
    /* stage 1 translation requires both ttbr 1 and ttbr 0
     * stage 2 translation requires ttbr 0*/
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    /*TCR2 is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TCR2, smmu_stage_table_config.tcr[1]);
    smmu_write_reg64(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TTBR1, smmu_stage_table_config.ttbr[1]);
#endif /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/

    /*ttbr0 (user space), for both stage 1 and stage 2*/
    smmu_write_reg64(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TTBR0, smmu_stage_table_config.ttbr[0]);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    /*MAIRs is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_MAIR0, smmu_stage_table_config.mair[0]);
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_MAIR1, smmu_stage_table_config.mair[1]);
#endif /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*SCTLR, */
    reg = CBn_SCTLR_CFIE | CBn_SCTLR_CFRE | CBn_SCTLR_AFE | CBn_SCTLR_TRE | CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_SCTLR, reg);
}

शून्य smmu_cb_disable(word_t cb, asid_t asid)
{

    uint32_t reg = smmu_read_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_SCTLR);
    reg &= ~CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_SCTLR, reg);
    smmu_tlb_invalidate_cb(cb, asid);
}

शून्य smmu_sid_bind_cb(word_t sid, word_t cb)
{

    uint32_t reg = 0;
    reg = S2CR_PRIVCFG_SET(S2CR_PRIVCFG_DEFAULT);
    reg |= S2CR_TYPE_SET(S2CR_TYPE_CB);
    reg |= S2CR_CBNDX_SET(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    /* The number of stream-to-context mapping
     * is related to the stream indexing method.
     * We currently supports mapping one stream ID to one context bank.*/
    जर (smmu_dev_knowledge.stream_match) {
        reg = SMR_VALID_SET(SMR_VALID_EN) | SMR_ID_SET(sid);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

शून्य smmu_sid_unbind(word_t sid)
{

    uint32_t reg =  S2CR_TYPE_SET(S2CR_TYPE_FAULT);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    जर (smmu_dev_knowledge.stream_match) {
        reg = SMR_VALID_SET(SMR_VALID_DIS);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

शून्य smmu_tlb_invalidate_all(शून्य)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /*on hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_MASK);
#अन्यथा
    /*on non-secure non-hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_MASK);
#endif
    /*syn above TLB operations*/
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
}

शून्य smmu_tlb_invalidate_cb(पूर्णांक cb, asid_t asid)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* SMMU uses a private VMID space. Each context bank assigns its VMID with its
     * context bnak number.*/
    uint32_t reg = TLBIVMID_SET(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIVMID, reg);
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
#अन्यथा
    /*stage 1*/
    uint32_t reg = CBn_TLBIASID_SET(asid);
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TLBIASID, reg);
    smmu_tlb_sync(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#endif
}

शून्य smmu_tlb_invalidate_cb_va(पूर्णांक cb, asid_t asid, vptr_t vaddr)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* invalidate all unlocated TLB entries in the stage 2 translation
    * associated with the given IPA*/
    uint64_t reg = CBn_TLBIIPAS2_SET(vaddr);
    smmu_write_reg64(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TLBIIPAS2, reg);
    smmu_tlb_sync(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#अन्यथा
    /*stage 1*/
    uint64_t reg = CBn_TLBIVA_SET(asid, vaddr);
    smmu_write_reg64(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TLBIVA, reg);
    smmu_tlb_sync(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#endif
}

शून्य smmu_read_fault_state(uint32_t *status, uint32_t *syndrome_0, uint32_t *syndrome_1)
{
    *status = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    *syndrome_0 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR0);
    *syndrome_1 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR1);
}

शून्य smmu_clear_fault_state(शून्य)
{
    uint32_t reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSR, reg);
}

शून्य smmu_cb_read_fault_state(पूर्णांक cb, uint32_t *status, word_t *address)
{
    *status = smmu_read_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_FSR);
    *address = smmu_read_reg64(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_FAR);
}

शून्य smmu_cb_clear_fault_state(पूर्णांक cb)
{
    smmu_write_reg32(SMMU_CBn_BASE_PPTR(cb), SMMU_CBn_FSR, CBn_FSR_CLEAR_ALL);
}

#endif /* CONFIG_ARM_SMMU */
