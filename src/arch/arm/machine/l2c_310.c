/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/machine/l2c_310.h>

#परिभाषय L2_पङ्क्तिः_आकार_द्वयङ्काः 5
#परिभाषय L2_पङ्क्तिः_आकार BIT(L2_पङ्क्तिः_आकार_द्वयङ्काः) /* 32 byte line size */

#परिभाषय L2_पङ्क्तिः_START(a) आवर्तनम्_अधस्तात्(a, L2_पङ्क्तिः_आकार_द्वयङ्काः)

compile_assert(l2_l1_same_पङ्क्तिः_आकार, L2_पङ्क्तिः_आकार_द्वयङ्काः == L1_CACHE_पङ्क्तिः_आकार_द्वयङ्काः)

/* MSHIELD Control */
#परिभाषय MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#परिभाषय MSHIELD_SMC_ROM_CTRL_AUX          0x109
#परिभाषय MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#परिभाषय MSHIELD_SMC_ROM_ADDR_FILT_START   /* ? */
#परिभाषय MSHIELD_SMC_ROM_ADDR_FILT_अन्तः     /* ? */
/* MSHIELD Control 2 */
#परिभाषय MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#परिभाषय MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#परिभाषय MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#परिभाषय MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#परिभाषय PL310_LOCKDOWN_BY_आवरणम्            (0xf<<25)
#परिभाषय PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#परिभाषय PL310_LOCKDOWN_BY_पङ्क्तिः            (0xf<<25)

/* Primary control */
#परिभाषय CTRL_CTRL_EN BIT(0)

/* Auxiliary control */
#परिभाषय CTRL_AUX_EARLY_BRESP_EN                            BIT(30)
#परिभाषय CTRL_AUX_IPREFETCH_EN                              BIT(29)
#परिभाषय CTRL_AUX_DPREFETCH_EN                              BIT(28)
#परिभाषय CTRL_AUX_NSECURE_INT_ACCESS                        BIT(27)
#परिभाषय CTRL_AUX_NSECURE_LOCKDOWN_EN                       BIT(26)
#परिभाषय CTRL_AUX_REPLACEMENT_POLICY                        BIT(25)
#परिभाषय CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * BIT(23)
#परिभाषय CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 BIT(22)
#परिभाषय CTRL_AUX_PARITY_EN                                 BIT(21)
#परिभाषय CTRL_AUX_EVENT_MONITOR_BUS_EN                      BIT(20)
#परिभाषय CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * BIT(17) )
#परिभाषय CTRL_AUX_ASSOCIATIVITY                             BIT(16)
#परिभाषय CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               BIT(13)
#परिभाषय CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    BIT(12)
#परिभाषय CTRL_AUX_STOREBUFDEV_सीमा_EN                      BIT(11)
#परिभाषय CTRL_AUX_उच्चः_PRIO_SODEV_EN                        BIT(10)
#परिभाषय CTRL_AUX_पूर्णम्_पङ्क्तिः_ZEROS_ENABLE                    BIT( 0)

#परिभाषय CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#परिभाषय CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#परिभाषय CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#परिभाषय CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#परिभाषय CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#परिभाषय CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#परिभाषय CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#परिभाषय CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#परिभाषय CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#परिभाषय CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#परिभाषय CTRL_RAM_LATENCY_समुच्चयः(X,S)    (((X)&0x7) * BIT(S))
#परिभाषय CTRL_RAM_LATENCY_सज्जता(X)    CTRL_RAM_LATENCY_समुच्चयः(X, 0)
#परिभाषय CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_समुच्चयः(X, 4)
#परिभाषय CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_समुच्चयः(X, 8)

#परिभाषय CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_सज्जता(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#परिभाषय MAINTENANCE_PENDING          BIT(0)

/* POWER */
#परिभाषय CTRL2_PWR_DYNAMIC_CLK_EN     BIT(1)
#परिभाषय CTRL2_PWR_STANDBY_ON         BIT(0)

/* PREFECTCH */
#परिभाषय CTRL2_PFET_DBL_LINEFILL_EN              BIT(30)
#परिभाषय CTRL2_PFET_INST_PREFETCH_EN             BIT(29)
#परिभाषय CTRL2_PFET_दत्त_PREFETCH_EN             BIT(28)
#परिभाषय CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      BIT(27)
#परिभाषय CTRL2_PFET_PREFETCH_DROP_EN             BIT(24)
#परिभाषय CTRL2_PFET_INCR_DBL_LINEFILL_EN         BIT(23)
#परिभाषय CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   BIT(21)
#परिभाषय CTRL2_PFET_PREFETCH_विस्थापनम्(X)    ((X) * BIT( 0) )


संरचना l2cc_map {

    संरचना {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    संरचना {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t चिह्नकम्_ram_control;       /* 0x108 */
        uint32_t दत्त_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    संरचना {
        uint32_t ev_गणक_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_आवरणम्;              /* 0x214 */
        uint32_t int_आवरणम्_स्थितिः;       /* 0x218 */
        uint32_t int_raw_स्थितिः;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } व्यवधानम् /* reg2 */;

    संरचना {
        uint32_t res[64];
    } reg3;
    संरचना {
        uint32_t res[64];
    } reg4;
    संरचना {
        uint32_t res[64];
    } reg5;
    संरचना {
        uint32_t res[64];
    } reg6;

    संरचना {
        uint32_t res[12];
        uint32_t cache_sync;            /* 0x730 */
        uint32_t res1[15];
        uint32_t inv_pa;                /* 0x770 */
        uint32_t res2[2];
        uint32_t inv_way;               /* 0x77C */
        uint32_t res3[12];
        uint32_t clean_pa;              /* 0x7B0 */
        uint32_t res4[1];
        uint32_t clean_सूचकांक;           /* 0x7B8 */
        uint32_t clean_way;             /* 0x7BC */
        uint32_t res5[12];
        uint32_t clean_inv_pa;          /* 0x7F0 */
        uint32_t res6[1];
        uint32_t clean_inv_सूचकांक;       /* 0x7F8 */
        uint32_t clean_inv_way;         /* 0x7FC */
    } maintenance /* reg7 */;

    संरचना {
        uint32_t res[64];
    } reg8;

    संरचना {
        uint32_t d_lockdown0;           /* 0x900 */
        uint32_t i_lockdown0;           /* 0x904 */
        uint32_t d_lockdown1;           /* 0x908 */
        uint32_t i_lockdown1;           /* 0x90C */
        uint32_t d_lockdown2;           /* 0x910 */
        uint32_t i_lockdown2;           /* 0x914 */
        uint32_t d_lockdown3;           /* 0x918 */
        uint32_t i_lockdown3;           /* 0x91C */
        uint32_t d_lockdown4;           /* 0x920 */
        uint32_t i_lockdown4;           /* 0x924 */
        uint32_t d_lockdown5;           /* 0x928 */
        uint32_t i_lockdown5;           /* 0x92C */
        uint32_t d_lockdown6;           /* 0x930 */
        uint32_t i_lockdown6;           /* 0x934 */
        uint32_t d_lockdown7;           /* 0x938 */
        uint32_t i_lockdown7;           /* 0x93C */
        uint32_t res[4];
        uint32_t तालकम्_पङ्क्तिः_eng;         /* 0x950 */
        uint32_t unlock_wayg;           /* 0x954 */
        uint32_t res1[42];
    } lockdown /* reg9 */;

    संरचना {
        uint32_t res[64];
    } reg10;
    संरचना {
        uint32_t res[64];
    } reg11;

    संरचना {
        uint32_t addr_filtering_start;  /* 0xC00 */
        uint32_t addr_filtering_अन्तः;    /* 0xC04 */
        uint32_t res[62];
    } filter /* reg12 */;

    संरचना {
        uint32_t res[64];
    } reg13;
    संरचना {
        uint32_t res[64];
    } reg14;

    संरचना {
        uint32_t res[16];
        uint32_t debug_ctrl;            /* 0xF40 */
        uint32_t res1[7];
        uint32_t prefetch_ctrl;         /* 0xF60 */
        uint32_t res2[7];
        uint32_t power_ctrl;            /* 0xF80 */
        uint32_t res3[31];
    } control2 /* reg15 */;
};


#यदि_अपरिभाषितम् L2CC_L2C310_PPTR
#दोषः L2CC_L2C310_PPTR must be defined प्रति आभास स्मृतिः access गन्तव्यम् the L2 cache controller
#अन्यथा  /* L2CC_PPTR */
चञ्चल संरचना l2cc_map *ध्रुव l2cc
    = (चञ्चल संरचना l2cc_map *)L2CC_L2C310_PPTR;
#यदि_अन्तः /* !L2CC_PPTR */


#यदि_परिभाषितम् TI_MSHIELD
BOOT_संहिता स्थिर शून्य mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
{
    पञ्जिका uint32_t _arg1 asm("r0") = arg1;
    पञ्जिका uint32_t _arg2 asm("r1") = arg2;
    पञ्जिका uint32_t _callid asm("r12") = callid;
    asm चञ्चल("push {r2-r12, lr}\n"
                 "dsb\n"
                 "smc #0\n"
                 "pop {r2-r12, lr}"
                 :: "r"(_callid), "r"(_arg1), "r"(_arg2));
}
#यदि_अन्तः /* TI_MSHIELD */

BOOT_संहिता शून्य initL2Cache(शून्य)
{
#यदि_अपरिभाषितम् CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t चिह्नकम्_ram;
    uint32_t दत्त_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#यदि_अपरिभाषितम् TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#यदि_अन्तः

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_दत्त_PREFETCH_EN;
#यदि defined(CONFIG_PLAT_IMX6)
    चिह्नकम्_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    दत्त_ram = CTRL_RAM_LATENCY(1, 2, 1);
#अन्यथा
    चिह्नकम्_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    दत्त_ram = CTRL_RAM_LATENCY(1, 2, 0);
#यदि_अन्तः

    aux      = 0
               | CTRL_AUX_IPREFETCH_EN
               | CTRL_AUX_DPREFETCH_EN
               | CTRL_AUX_NSECURE_INT_ACCESS
               | CTRL_AUX_NSECURE_LOCKDOWN_EN
               | CTRL_AUX_ASSOCIATIVITY_16WAY
               | CTRL_AUS_REPLPOLICY_RROBIN;

#यदि defined(CONFIG_PLAT_IMX6SX)
    aux |= CTRL_AUX_WAYSIZE_16K;
#नोचेद्यदि defined(CONFIG_PLAT_EXYNOS4) || defined(CONFIG_PLAT_IMX6) || defined(CONFIG_PLAT_ZYNQ7000) || defined(CONFIG_PLAT_ALLWINNERA20)
    aux |= CTRL_AUX_WAYSIZE_64K;
#नोचेद्यदि defined(OMAP4)
    aux |= CTRL_AUX_WAYSIZE_32K;
#अन्यथा /* ! (EXYNOS4 || OMAP4 || IMX6) */
#दोषः Unknown platform प्रति L2C-310
#यदि_अन्तः /* EXYNOS4 || OMAP4 || IMX6 */

#यदि_परिभाषितम् TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, चिह्नकम्_ram, दत्त_ram);

#अन्यथा /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.चिह्नकम्_ram_control  = चिह्नकम्_ram;
    l2cc->control.दत्त_ram_control = दत्त_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#यदि_अन्तः /* TI_MSHIELD */

    /* 2: Invalidate by way. */
    l2cc->maintenance.inv_way = 0xffff;
    यावत् (l2cc->maintenance.inv_way & 0xffff);

    /* 3: write to lockdown D & I reg9 if required  */
    यदि ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_आवरणम्) == PL310_LOCKDOWN_BY_MASTER) {
        /* disable lockdown */
        l2cc->lockdown.d_lockdown0 = 0;
        l2cc->lockdown.i_lockdown0 = 0;
        l2cc->lockdown.d_lockdown1 = 0;
        l2cc->lockdown.i_lockdown1 = 0;
        l2cc->lockdown.d_lockdown2 = 0;
        l2cc->lockdown.i_lockdown2 = 0;
        l2cc->lockdown.d_lockdown3 = 0;
        l2cc->lockdown.i_lockdown3 = 0;
        l2cc->lockdown.d_lockdown4 = 0;
        l2cc->lockdown.i_lockdown4 = 0;
        l2cc->lockdown.d_lockdown5 = 0;
        l2cc->lockdown.i_lockdown5 = 0;
        l2cc->lockdown.d_lockdown6 = 0;
        l2cc->lockdown.i_lockdown6 = 0;
        l2cc->lockdown.d_lockdown7 = 0;
        l2cc->lockdown.i_lockdown7 = 0;
    }
    यदि ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_आवरणम्) == PL310_LOCKDOWN_BY_पङ्क्तिः) {
        /* disable lockdown */
        l2cc->lockdown.तालकम्_पङ्क्तिः_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->व्यवधानम्.int_आवरणम्  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->व्यवधानम्.int_clear = आवरणम्(9);

    /* 6: Enable the L2 cache */
#यदि_परिभाषितम् TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#अन्यथा /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#यदि_अन्तः /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#यदि defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    समुच्चयः_ACTLR(getACTLR() | BIT(1));
#यदि_अन्तः

#यदि_अन्तः /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

स्थिर अन्तःस्थ शून्य L2_cacheSync(शून्य)
{
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    l2cc->maintenance.cache_sync = 0;
    यावत् (l2cc->maintenance.cache_sync & MAINTENANCE_PENDING);
    /* Wait till the L2 cache operation has completed and avoid later memoy
     * operations being reordered to before this point */
    dsb();
}

/*
 * Errata 727915 applies for PL310 revisions r2p0, r3p0, fixed in r3p1.
 * i.MX6 has r3p1, so is not affected.
 * If a new write arrives during the clean & invalidate operation,
 * there is a small chance it will get lost.
 * The same is true when doing separate clean and invalidate operations,
 * hence that is not a proper work-around and worse than the errata.
 */
शून्य plat_cleanInvalidateL2Cache(शून्य)
{
    यदि (!config_समुच्चयः(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        यावत् (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

शून्य plat_cleanCache(शून्य)
{
#यदि_अपरिभाषितम् CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    यावत् (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#यदि_अन्तः /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

शून्य plat_cleanL2Range(paddr_t start, paddr_t अन्तः_2)
{
#यदि_अपरिभाषितम् CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    प्रतिज्ञा(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    प्रति (start = L2_पङ्क्तिः_START(start);
         start != L2_पङ्क्तिः_START(अन्तः_2 + L2_पङ्क्तिः_आकार);
         start += L2_पङ्क्तिः_आकार) {
        l2cc->maintenance.clean_pa = start;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#यदि_अन्तः /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

शून्य plat_invalidateL2Range(paddr_t start, paddr_t अन्तः_2)
{
#यदि_अपरिभाषितम् CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    प्रतिज्ञा(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    प्रति (start = L2_पङ्क्तिः_START(start);
         start != L2_पङ्क्तिः_START(अन्तः_2 + L2_पङ्क्तिः_आकार);
         start += L2_पङ्क्तिः_आकार) {
        l2cc->maintenance.inv_pa = start;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#यदि_अन्तः /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

शून्य plat_cleanInvalidateL2Range(paddr_t start, paddr_t अन्तः_2)
{
#यदि_अपरिभाषितम् CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    प्रतिज्ञा(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    प्रति (start = L2_पङ्क्तिः_START(start);
         start != L2_पङ्क्तिः_START(अन्तः_2 + L2_पङ्क्तिः_आकार);
         start += L2_पङ्क्तिः_आकार) {
        l2cc->maintenance.clean_inv_pa = start;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#यदि_अन्तः /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

