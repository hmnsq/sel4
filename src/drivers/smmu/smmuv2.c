/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>
#ಅನ್ತರ್ಭಾವ <drivers/smmu/smmuv2.h>

/*supported stages of translations*/
#ವ್ಯಾಖ್ಯಾನಿಸು STAGE1_TRANS           (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು STAGE2_TRANS           (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು NESTED_TRANS           (1 << 2)
/*supported translation table formats*/
#ವ್ಯಾಖ್ಯಾನಿಸು AARCH32S_FMT           (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು AARCH32L_FMT           (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು NO_AARCH32_FMT         (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು TRANS_ಪುಟಗಳು_4_KB        (1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು TRANS_ಪುಟಗಳು_16_KB       (1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು TRANS_ಪುಟಗಳು_64_KB       (1 << 5)

/*the default virtual address bits for partition TTBR0 and TTBR1*/
#ವ್ಯಾಖ್ಯಾನಿಸು SMMU_VA_DEFAULT_ಬಿಟ್ಟುಗಳು      48

ರಚನೆ  smmu_feature {
    bool_t ಹರಿವು_ಹೊಂದಿಕೆ;              /*stream match register functionality included*/
    bool_t trans_op;                  /*address translation operations supported*/
    bool_t cotable_walk;              /*coherent translation table walk*/
    bool_t ಪ್ರಸಾರ_tlb;             /*broadcast TLB maintenance*/
    bool_t vmid16;                    /*16 bits VMIDs are supported*/
    uint32_t supported_trans;         /*supported translation stages*/
    uint32_t supported_fmt;           /*supported translation formats*/
    uint32_t ಅಂಕೆ_cfault_ints;         /*supported number of context fault interrupts*/
    uint32_t ಅಂಕೆ_ಹರಿವು_ids;          /*number of stream IDs*/
    uint32_t ಅಂಕೆ_ಹರಿವು_map_groups;   /*num stream mapping register groups*/
    uint32_t smmu_ಪುಟ_ಗಾತ್ರ;          /*page size in SMMU register address space*/
    uint32_t smmu_ಅಂಕೆ_ಪುಟಗಳು;          /*number of pages in global or context bank address space*/
    uint32_t ಅಂಕೆ_s2_cbanks;           /*cbanks that support stage 2 only*/
    uint32_t ಅಂಕೆ_cbanks;              /*total number of context banks*/
    uint32_t va_ಬಿಟ್ಟುಗಳು;                 /*upstream address size*/
    uint32_t pa_ಬಿಟ್ಟುಗಳು;                 /*PA address size*/
    uint32_t ipa_ಬಿಟ್ಟುಗಳು;                /*IPA address size*/
    pptr_t cb_ತಳ;                   /*base of context bank address space*/
};

ರಚನೆ smmu_ಕೋಷ್ಟಕ_config {
    uint32_t tcr[2];                    /*SMMU_CBn_TCRm*/
    uint32_t mair[2];                  /*SMMU_CBn_MAIRm*/
    uint64_t ttbr[2];                  /*SMMU_CBn_TTBRm*/
};

ಸ್ಥಿರ ರಚನೆ smmu_feature smmu_dev_knowledge;
ಸ್ಥಿರ ರಚನೆ smmu_ಕೋಷ್ಟಕ_config smmu_stage_ಕೋಷ್ಟಕ_config;


ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t smmu_read_reg32(pptr_t ತಳ, uint32_t ಸೂಚ್ಯಂಕ)
{
    ಹಿಂತಿರುಗಿ *(ಅಸ್ಥಿರ uint32_t *)(ತಳ + ಸೂಚ್ಯಂಕ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ smmu_write_reg32(pptr_t ತಳ, uint32_t ಸೂಚ್ಯಂಕ, uint32_t val)
{
    *(ಅಸ್ಥಿರ uint32_t *)(ತಳ + ಸೂಚ್ಯಂಕ) = val;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t smmu_read_reg64(pptr_t ತಳ, uint32_t ಸೂಚ್ಯಂಕ)
{
    ಹಿಂತಿರುಗಿ *(ಅಸ್ಥಿರ uint64_t *)(ತಳ + ಸೂಚ್ಯಂಕ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ smmu_write_reg64(pptr_t ತಳ, uint32_t ಸೂಚ್ಯಂಕ, uint64_t val)
{
    *(ಅಸ್ಥಿರ uint64_t *)(ತಳ + ಸೂಚ್ಯಂಕ) = val;
}

ಸ್ಥಿರ ಶೂನ್ಯ smmu_tlb_sync(pptr_t ತಳ, uint32_t sync, uint32_t ಸ್ಥಿತಿ_2)
{
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
    smmu_write_reg32(ತಳ, sync, SMMU_TLB_SYNC_ಮುಖವಾಡ);
    ಆಗ (ಎಣಿಕೆ_2 < TLBSYNC_LOOP) {
        /*pulling the active flag, reading the TLB command state.*/
        ಆದರೆ (!(smmu_read_reg32(ತಳ, ಸ್ಥಿತಿ_2) & TLBSTATUS_GSACTIVE)) {
            ನಿಲ್ಲಿಸು;
        }
        ಎಣಿಕೆ_2++;
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t smmu_obs_ಗಾತ್ರ_ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು(uint32_t ಗಾತ್ರ_2)
{
    /*converting the output bus address size into address bit, defined in
    IDx registers*/
    ಆಯ್ಕೆ (ಗಾತ್ರ_2) {
    ಸ್ಥಿತಿ 0:
        ಹಿಂತಿರುಗಿ 32;
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ 36;
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ 40;
    ಸ್ಥಿತಿ 3:
        ಹಿಂತಿರುಗಿ 42;
    ಸ್ಥಿತಿ 4:
        ಹಿಂತಿರುಗಿ 44;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 48;
    }
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t smmu_ubs_ಗಾತ್ರ_ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು(uint32_t ಗಾತ್ರ_2)
{
    /*converting the upstream address size into address bit, defined in
    IDx registers*/
    ಆಯ್ಕೆ (ಗಾತ್ರ_2) {
    ಸ್ಥಿತಿ 0:
        ಹಿಂತಿರುಗಿ 32;
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ 36;
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ 40;
    ಸ್ಥಿತಿ 3:
        ಹಿಂತಿರುಗಿ 42;
    ಸ್ಥಿತಿ 4:
        ಹಿಂತಿರುಗಿ 44;
    ಸ್ಥಿತಿ 5:
        ಹಿಂತಿರುಗಿ 49;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 64;
    }
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ smmu_mapping_init(ಶೂನ್ಯ)
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
    map_kernel_ಚೌಕಟ್ಟು(SMMU_GR1_PADDR(smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ),
                     SMMU_GR1_PPTR,
                     VMKernelOnly,
                     vm_ಗುಣಲಕ್ಷಣಗಳು_new(true, false, false));
    /*GID registers*/
    map_kernel_ಚೌಕಟ್ಟು(SMMU_GID_PADDR(smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ),
                     SMMU_GID_PPTR,
                     VMKernelOnly,
                     vm_ಗುಣಲಕ್ಷಣಗಳು_new(true, false, false));
    /*PM registers*/
    map_kernel_ಚೌಕಟ್ಟು(SMMU_PM_PADDR(smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ),
                     SMMU_PM_PPTR,
                     VMKernelOnly,
                     vm_ಗುಣಲಕ್ಷಣಗಳು_new(true, false, false));
    /*SSD registers*/
    map_kernel_ಚೌಕಟ್ಟು(SMMU_SSD_PADDR(smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ),
                     SMMU_SSD_PPTR,
                     VMKernelOnly,
                     vm_ಗುಣಲಕ್ಷಣಗಳು_new(true, false, false));
    /*map the context banks, each bank maps to a 4K page*/
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < smmu_dev_knowledge.ಅಂಕೆ_cbanks; i++) {
        map_kernel_ಚೌಕಟ್ಟು(SMMU_CBn_PADDR(smmu_dev_knowledge.cb_ತಳ, i, smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ),
                         SMMU_CBn_ತಳ_PPTR(i),
                         VMKernelOnly,
                         vm_ಗುಣಲಕ್ಷಣಗಳು_new(true, false, false));
    }
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ smmu_config_prob(ಶೂನ್ಯ)
{
    uint32_t reg, ಕ್ಷೇತ್ರ;
    /*ID0*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR0);
    /*stages supported*/
    ಆದರೆ (reg & IDR0_S1TS) {
        smmu_dev_knowledge.supported_trans |= STAGE1_TRANS;
    }
    ಆದರೆ (reg & IDR0_S2TS) {
        smmu_dev_knowledge.supported_trans |= STAGE2_TRANS;
    }
    ಆದರೆ (reg & IDR0_NTS) {
        smmu_dev_knowledge.supported_trans |= NESTED_TRANS;
    }
    /*stream matching register*/
    ಆದರೆ (reg & IDR0_SMS) {
        smmu_dev_knowledge.ಹರಿವು_ಹೊಂದಿಕೆ = true;
    }
    /*address translation operation*/
    ಆದರೆ ((reg & IDR0_ATOSNS) == 0) {
        smmu_dev_knowledge.trans_op = true;
    }
    /*AARCH32 translation format support*/
    ಕ್ಷೇತ್ರ = IDR0_PTFS_VAL(reg & IDR0_PTFS);
    ಆದರೆ (ಕ್ಷೇತ್ರ == PTFS_AARCH32S_AARCH32L) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
        smmu_dev_knowledge.supported_fmt |= AARCH32S_FMT;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕ್ಷೇತ್ರ == PTFS_AARCH32L_ONLY) {
        smmu_dev_knowledge.supported_fmt |= AARCH32L_FMT;
    } ಇಲ್ಲದಿದ್ದರೆ {
        smmu_dev_knowledge.supported_fmt |= NO_AARCH32_FMT;
    }
    /*number of context fault interrupts
    * However, in smmuv2, each context bank has dedicated interrupt pin
    * hence no requirement to specify implemented interrupts here.*/
    smmu_dev_knowledge.ಅಂಕೆ_cfault_ints = IDR0_NUMIRPT_VAL(reg & IDR0_NUMIRPT);
    /*coherent translation table walk*/
    ಆದರೆ (reg & IDR0_CTTW) {
        smmu_dev_knowledge.cotable_walk = true;
    }
    /*broadcast TLB maintenance*/
    ಆದರೆ (reg & IDR0_BTM) {
        smmu_dev_knowledge.ಪ್ರಸಾರ_tlb = true;
    }
    /*number of stream IDs*/
    smmu_dev_knowledge.ಅಂಕೆ_ಹರಿವು_ids = (1 << IDR0_NUMSIDB_VAL(reg & IDR0_NUMSIDB)) - 1;
    /*number of stream mapping register groups*/
    smmu_dev_knowledge.ಅಂಕೆ_ಹರಿವು_map_groups = reg & IDR0_NUMSMRG;

    /*ID1*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR1);
    /*smmu page size*/
    ಆದರೆ (reg & IDR1_PAGESIZE) {
        smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ = SMMU_ಪುಟ_64_KB;
    } ಇಲ್ಲದಿದ್ದರೆ {
        smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ = SMMU_ಪುಟ_4_KB;
    }
    /*smmu num pages, 2^(numdxb + 1)*/
    ಕ್ಷೇತ್ರ = IDR1_NUMPAGENDXB_VAL(reg & IDR1_NUMPAGENDXB);
    smmu_dev_knowledge.smmu_ಅಂಕೆ_ಪುಟಗಳು = 1 << (ಕ್ಷೇತ್ರ + 1);
    /*num of stage 2 context banks*/
    smmu_dev_knowledge.ಅಂಕೆ_s2_cbanks = IDR1_NUMS2CB_VAL(reg & IDR1_NUMS2CB);
    /*total num of context banks*/
    smmu_dev_knowledge.ಅಂಕೆ_cbanks = reg & IDR1_NUMCB;
    /*calculate the context bank base*/
    smmu_dev_knowledge.cb_ತಳ = SMMU_CB_ತಳ_PADDR(
                                     SMMU_GLOBAL_ಗಾತ್ರ(smmu_dev_knowledge.smmu_ಅಂಕೆ_ಪುಟಗಳು, smmu_dev_knowledge.smmu_ಪುಟ_ಗಾತ್ರ));

    /*ID2*/
    reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_IDR2);
    /*VNID16S*/
    ಆದರೆ (reg & IDR2_VMID16S) {
        smmu_dev_knowledge.vmid16 = true;
    }
    /*PTFSV8_64KB*/
    ಆದರೆ (reg & IDR2_PTFSV8_64) {
        smmu_dev_knowledge.supported_fmt |= TRANS_ಪುಟಗಳು_64_KB;
    }
    /*PTFSV8_16KB*/
    ಆದರೆ (reg & IDR2_PTFSV8_16) {
        smmu_dev_knowledge.supported_fmt |= TRANS_ಪುಟಗಳು_16_KB;
    }
    /*PTFSV8_64KB*/

    ಆದರೆ (reg & IDR2_PTFSV8_4) {
        smmu_dev_knowledge.supported_fmt |= TRANS_ಪುಟಗಳು_4_KB;
    }
    /*UBS virtual address size*/
    smmu_dev_knowledge.va_ಬಿಟ್ಟುಗಳು = smmu_ubs_ಗಾತ್ರ_ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು(IDR2_UBS_VAL(reg & IDR2_UBS));
    /*OAS*/
    smmu_dev_knowledge.pa_ಬಿಟ್ಟುಗಳು = smmu_obs_ಗಾತ್ರ_ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು(IDR2_OAS_VAL(reg & IDR2_OAS));
    /*IAS*/
    smmu_dev_knowledge.ipa_ಬಿಟ್ಟುಗಳು = smmu_obs_ಗಾತ್ರ_ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು(reg & IDR2_IAS);
}



BOOT_ಸಂಕೇತ  ಸ್ಥಿರ ಶೂನ್ಯ smmu_dev_reset(ಶೂನ್ಯ)
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
    reg = S2_CR_PRIVCFG_ಗಣ(S2CR_PRIVCFG_DEFAULT);
    reg |= S2_CR_TYPE_ಗಣ(S2CR_TYPE_CB);

    /*the number of stream-to-context is related to the stream indexing method*/
    ಆದರೆ (smmu_dev_knowledge.ಹರಿವು_ಹೊಂದಿಕೆ) {
        /*stream matching*/
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < smmu_dev_knowledge.ಅಂಕೆ_ಹರಿವು_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(i), reg);
        }
        /*reset the stream match registers as invalid*/
        reg = SMR_VALID_ಗಣ(SMR_VALID_DIS);
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < smmu_dev_knowledge.ಅಂಕೆ_ಹರಿವು_map_groups; i++) {
            smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(i), reg);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*stream ID*/
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < smmu_dev_knowledge.ಅಂಕೆ_ಹರಿವು_ids; i++) {
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
    ಆದರೆ (major >= 2) {
        reg &= ~ACR_CACHE_ಬೀಗ;
    }
    /*enable the TLB to cache bypassing*/
    reg |= ACR_S2CRB_TLBEN | ACR_SMTNMB_TLBEN;
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sACR, reg);
    /*special init requested by the smmu-500: end*/

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < smmu_dev_knowledge.ಅಂಕೆ_cbanks; i++) {
        cb_bank_ptr = SMMU_CBn_ತಳ_PPTR(i);
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
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_ಮುಖವಾಡ);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_ಮುಖವಾಡ);

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

BOOT_ಸಂಕೇತ ಶೂನ್ಯ plat_smmu_init(ಶೂನ್ಯ)
{
    smmu_config_prob();
    smmu_mapping_init();
    smmu_dev_reset();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ ಶೂನ್ಯ smmu_config_stage2(ರಚನೆ smmu_ಕೋಷ್ಟಕ_config *cfg,
                               vspace_ಬೇರು_t *vspace)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    reg |= CBn_TCR_SH0_ಗಣ(CBn_TCR_SH_INNER);
    reg |= CBn_TCR_ORGN0_ಗಣ(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_IRGN0_ಗಣ(CBn_TCR_GN_WB_WA_CACHE);
    reg |= CBn_TCR_TG0_ಗಣ(CBn_TCR_TG_4K);
    /*setting according to the vcpu_init_vtcr in vcpu.h*/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_PA_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು_40
    reg |= CBn_TCR_T0_SZ_ಗಣ(24);
    reg |= CBn_TCR_PASize_ಗಣ(CBn_TCR2_PASize_40);
    reg |= CBn_TCR_SL0_ಗಣ(CBn_TCR_SL0_4KB_L1);
#ಇಲ್ಲದಿದ್ದರೆ
    reg |= CBn_TCR_T0_SZ_ಗಣ(20);
    reg |= CBn_TCR_PASize_ಗಣ(CBn_TCR2_PASize_44);
    reg |= CBn_TCR_SL0_ಗಣ(CBn_TCR_SL0_4KB_L0);
#ಆದರೆ_ಅಂತ್ಯ
    /*reserved as 1*/
    reg |= ಬಿಟ್(31);
    cfg->tcr[0] = reg;
    /*vttbr*/
    cfg->ttbr[0] = ttbr_new(0, pptr_ಗಮ್ಯ_paddr(vspace)).ಪದಗಳು[0];
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ smmu_config_stage1(ರಚನೆ smmu_ಕೋಷ್ಟಕ_config *cfg,
                               bool_t coherence, uint32_t pa_ಬಿಟ್ಟುಗಳು,
                               vspace_ಬೇರು_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    /*SMMU_CBn_TCR*/
    ಆದರೆ (coherence) {
        reg |= CBn_TCR_SH0_ಗಣ(CBn_TCR_SH_INNER);
        reg |= CBn_TCR_ORGN0_ಗಣ(CBn_TCR_GN_WB_WA_CACHE);
        reg |= CBn_TCR_IRGN0_ಗಣ(CBn_TCR_GN_WB_WA_CACHE);
    } ಇಲ್ಲದಿದ್ದರೆ {
        reg |= CBn_TCR_SH0_ಗಣ(CBn_TCR_SH_OUTER);
        reg |= CBn_TCR_ORGN0_ಗಣ(CBn_TCR_GN_NCACHE);
        reg |= CBn_TCR_IRGN0_ಗಣ(CBn_TCR_GN_NCACHE);
    }
    /*page size is configured as 4k*/
    reg |= CBn_TCR_TG0_ಗಣ(CBn_TCR_TG_4K);
    /*the TTBR0 size, calculated according to the aarch64 formula*/
    reg |= CBn_TCR_T0_SZ_ಗಣ(64 - SMMU_VA_DEFAULT_ಬಿಟ್ಟುಗಳು);
    /*disable (speculative) page table walks through TTBR1*/
    reg |= CBn_TCR_EPD1_DIS;
    cfg->tcr[0] = reg;
    /*TCR2*/
    reg = 0;
    ಆಯ್ಕೆ (pa_ಬಿಟ್ಟುಗಳು) {
    ಸ್ಥಿತಿ 32:
        reg |= CBn_TCR2_PASize_ಗಣ(CBn_TCR2_PASize_32);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 36:
        reg |= CBn_TCR2_PASize_ಗಣ(CBn_TCR2_PASize_36);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 40:
        reg |= CBn_TCR2_PASize_ಗಣ(CBn_TCR2_PASize_40);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 42:
        reg |= CBn_TCR2_PASize_ಗಣ(CBn_TCR2_PASize_42);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 44:
        reg |= CBn_TCR2_PASize_ಗಣ(CBn_TCR2_PASize_44);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        reg |= CBn_TCR2_PASize_ಗಣ(CBn_TCR2_PASize_48);
        ನಿಲ್ಲಿಸು;
    }
    /*currently only support AArch64*/
    reg |= CBn_TCR2_SEP_ಗಣ(CBn_TCR2_SEP_UPSTREAM_ಗಾತ್ರ) | CBn_TCR2_AS_ಗಣ(CBn_TCR2_AS_16);
    cfg->tcr[1] = reg;
    /*MAIR0, configured according to the MAIR values in cores*/
    reg = CBn_MAIRm_ATTR_DEVICE_nGnRnE << CBn_MAIRm_ATTR_ಸರಿಸು(CBn_MAIRm_ATTR_ID_DEVICE_nGnRnE);
    reg |= CBn_MAIRm_ATTR_DEVICE_nGnRE << CBn_MAIRm_ATTR_ಸರಿಸು(CBn_MAIRm_ATTR_ID_DEVICE_nGnRE);
    reg |= CBn_MAIRm_ATTR_DEVICE_GRE << CBn_MAIRm_ATTR_ಸರಿಸು(CBn_MAIRm_ATTR_ID_DEVICE_GRE);
    reg |= CBn_MAIRm_ATTR_NC << CBn_MAIRm_ATTR_ಸರಿಸು(CBn_MAIRm_ATTR_ID_NC);
    cfg->mair[0] = reg;
    /*MAIR1*/
    reg = CBn_MAIRm_ATTR_CACHE << CBn_MAIRm_ATTR_ಸರಿಸು(CBn_MAIRm_ATTR_ID_CACHE);
    cfg->mair[1] = reg;
    /*TTBRs*/
    /*The SMMU only uses user-level address space, TTBR0.*/
    cfg->ttbr[0] = ttbr_new(asid, pptr_ಗಮ್ಯ_paddr(vspace)).ಪದಗಳು[0];
    cfg->ttbr[1] = 0;
}
#ಆದರೆ_ಅಂತ್ಯ /*CONFIG_ARM_HYPERVISOR_SUPPORT*/


ಶೂನ್ಯ smmu_cb_ನಿಯೋಜಿಸು_vspace(ಶಬ್ದ_t cb, vspace_ಬೇರು_t *vspace, asid_t asid)
{
    uint32_t reg = 0;
    uint32_t vmid = cb;
    /* For the stage 2 translation, the VMID space is designed as a private
     * space, its value is equal to the context bank index. Using private VMID
     * space avoids synchronising with vspace management on VMID reallocations.
     * Also, VMID used by SMMU need to be valid all time once device transactions
     * are enabled. To maintain the TLB coherency, we introduces a set of mechanism
      * that connects vspace to context banks linked via ASID. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    smmu_config_stage2(&smmu_stage_ಕೋಷ್ಟಕ_config,
                       vspace);
#ಇಲ್ಲದಿದ್ದರೆ
    smmu_config_stage1(&smmu_stage_ಕೋಷ್ಟಕ_config,
                       smmu_dev_knowledge.cotable_walk,
                       smmu_dev_knowledge.ipa_ಬಿಟ್ಟುಗಳು,
                       vspace,
                       asid);
#ಆದರೆ_ಅಂತ್ಯ /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*CBA2R*/
    /*currently only support aarch64*/
    reg = CBA2_Rn_VA64_ಗಣ;
    ಆದರೆ (smmu_dev_knowledge.vmid16) {
        reg |= CBA2_Rn_VMID_ಗಣ(vmid);
    }
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBA2Rn(cb), reg);

    /*CBAR*/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2 translation only, CBAR_TYPE_S2_TRANS*/
    reg = CBARn_TYPE_ಗಣ(CBARn_TYPE_STAGE2);
    /*8 bit VMID*/
    ಆದರೆ (!smmu_dev_knowledge.vmid16) {
        reg |= CBARn_VMID_ಗಣ(vmid);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    /*stage 1 translation only, CBAR_TYPE_S1_TRANS_S2_BYPASS*/
    reg = CBARn_TYPE_ಗಣ(CBARn_TYPE_STAGE1);
    /*configured as the weakest shareability/memory types,
     * so they can be overwritten by ttbcr or pte */
    reg |= CBARn_BPSHCFG_ಗಣ(CBARn_BPSHCFG_NONE);
    reg |= CBARn_Mem_Attr_ಗಣ(MemAttr_OWB_IWB);
#ಆದರೆ_ಅಂತ್ಯ  /*CONFIG_ARM_HYPERVISOR_SUPPORT*/
    smmu_write_reg32(SMMU_GR1_PPTR, SMMU_CBARn(cb), reg);
    /*TCR*/
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TCR, smmu_stage_ಕೋಷ್ಟಕ_config.tcr[0]);
    /* stage 1 translation requires both ttbr 1 and ttbr 0
     * stage 2 translation requires ttbr 0*/
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /*TCR2 is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TCR2, smmu_stage_ಕೋಷ್ಟಕ_config.tcr[1]);
    smmu_write_reg64(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TTBR1, smmu_stage_ಕೋಷ್ಟಕ_config.ttbr[1]);
#ಆದರೆ_ಅಂತ್ಯ /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/

    /*ttbr0 (user space), for both stage 1 and stage 2*/
    smmu_write_reg64(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TTBR0, smmu_stage_ಕೋಷ್ಟಕ_config.ttbr[0]);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /*MAIRs is required by stage 1 only*/
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_MAIR0, smmu_stage_ಕೋಷ್ಟಕ_config.mair[0]);
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_MAIR1, smmu_stage_ಕೋಷ್ಟಕ_config.mair[1]);
#ಆದರೆ_ಅಂತ್ಯ /*!CONFIG_ARM_HYPERVISOR_SUPPORT*/
    /*SCTLR, */
    reg = CBn_SCTLR_CFIE | CBn_SCTLR_CFRE | CBn_SCTLR_AFE | CBn_SCTLR_TRE | CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_SCTLR, reg);
}

ಶೂನ್ಯ smmu_cb_disable(ಶಬ್ದ_t cb, asid_t asid)
{

    uint32_t reg = smmu_read_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_SCTLR);
    reg &= ~CBn_SCTLR_M;
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_SCTLR, reg);
    smmu_tlb_invalidate_cb(cb, asid);
}

ಶೂನ್ಯ smmu_sid_bind_cb(ಶಬ್ದ_t sid, ಶಬ್ದ_t cb)
{

    uint32_t reg = 0;
    reg = S2_CR_PRIVCFG_ಗಣ(S2CR_PRIVCFG_DEFAULT);
    reg |= S2_CR_TYPE_ಗಣ(S2CR_TYPE_CB);
    reg |= S2_CR_CBNDX_ಗಣ(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    /* The number of stream-to-context mapping
     * is related to the stream indexing method.
     * We currently supports mapping one stream ID to one context bank.*/
    ಆದರೆ (smmu_dev_knowledge.ಹರಿವು_ಹೊಂದಿಕೆ) {
        reg = SMR_VALID_ಗಣ(SMR_VALID_EN) | SMR_ID_ಗಣ(sid);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

ಶೂನ್ಯ smmu_sid_unbind(ಶಬ್ದ_t sid)
{

    uint32_t reg =  S2_CR_TYPE_ಗಣ(S2CR_TYPE_FAULT);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_S2CRn(sid), reg);
    ಆದರೆ (smmu_dev_knowledge.ಹರಿವು_ಹೊಂದಿಕೆ) {
        reg = SMR_VALID_ಗಣ(SMR_VALID_DIS);
        smmu_write_reg32(SMMU_GR0_PPTR, SMMU_SMRn(sid), reg);
    }
}

ಶೂನ್ಯ smmu_tlb_invalidate_all(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /*on hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLH, SMMU_TLB_INVALL_ಮುಖವಾಡ);
#ಇಲ್ಲದಿದ್ದರೆ
    /*on non-secure non-hyp entries*/
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIALLNSNH, SMMU_TLB_INVALL_ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ
    /*syn above TLB operations*/
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
}

ಶೂನ್ಯ smmu_tlb_invalidate_cb(ಪೂರ್ಣಾಂಕ cb, asid_t asid)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* SMMU uses a private VMID space. Each context bank assigns its VMID with its
     * context bnak number.*/
    uint32_t reg = TLBIVMID_ಗಣ(cb);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_TLBIVMID, reg);
    smmu_tlb_sync(SMMU_GR0_PPTR, SMMU_sTLBGSYNC, SMMU_sTLBGSTATUS);
#ಇಲ್ಲದಿದ್ದರೆ
    /*stage 1*/
    uint32_t reg = CBn_TLBIASID_ಗಣ(asid);
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TLBIASID, reg);
    smmu_tlb_sync(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ smmu_tlb_invalidate_cb_va(ಪೂರ್ಣಾಂಕ cb, asid_t asid, vptr_t vaddr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /*stage 2*/
    /* invalidate all unlocated TLB entries in the stage 2 translation
    * associated with the given IPA*/
    uint64_t reg = CBn_TLBIIPAS2_ಗಣ(vaddr);
    smmu_write_reg64(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TLBIIPAS2, reg);
    smmu_tlb_sync(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#ಇಲ್ಲದಿದ್ದರೆ
    /*stage 1*/
    uint64_t reg = CBn_TLBIVA_ಗಣ(asid, vaddr);
    smmu_write_reg64(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TLBIVA, reg);
    smmu_tlb_sync(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_TLBSYNC, SMMU_CBn_TLBSTATUS);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ smmu_read_fault_state(uint32_t *ಸ್ಥಿತಿ_2, uint32_t *syndrome_0, uint32_t *syndrome_1)
{
    *ಸ್ಥಿತಿ_2 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    *syndrome_0 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR0);
    *syndrome_1 = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSYNR1);
}

ಶೂನ್ಯ smmu_clear_fault_state(ಶೂನ್ಯ)
{
    uint32_t reg = smmu_read_reg32(SMMU_GR0_PPTR, SMMU_sGFSR);
    smmu_write_reg32(SMMU_GR0_PPTR, SMMU_sGFSR, reg);
}

ಶೂನ್ಯ smmu_cb_read_fault_state(ಪೂರ್ಣಾಂಕ cb, uint32_t *ಸ್ಥಿತಿ_2, ಶಬ್ದ_t *ವಿಳಾಸ)
{
    *ಸ್ಥಿತಿ_2 = smmu_read_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_FSR);
    *ವಿಳಾಸ = smmu_read_reg64(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_FAR);
}

ಶೂನ್ಯ smmu_cb_clear_fault_state(ಪೂರ್ಣಾಂಕ cb)
{
    smmu_write_reg32(SMMU_CBn_ತಳ_PPTR(cb), SMMU_CBn_FSR, CBn_FSR_CLEAR_ALL);
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_SMMU */
