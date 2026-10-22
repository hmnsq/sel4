/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/l2c_310.h>

#تعریف_کړه L2_کرښه_اندازه_بټونه 5
#تعریف_کړه L2_کرښه_اندازه بټ(L2_کرښه_اندازه_بټونه) /* 32 byte line size */

#تعریف_کړه L2_کرښه_پیل(a) پړاو_ښکته(a, L2_کرښه_اندازه_بټونه)

compile_assert(l2_l1_same_کرښه_اندازه, L2_کرښه_اندازه_بټونه == L1_CACHE_کرښه_اندازه_بټونه)

/* MSHIELD Control */
#تعریف_کړه MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#تعریف_کړه MSHIELD_SMC_ROM_CTRL_AUX          0x109
#تعریف_کړه MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#تعریف_کړه MSHIELD_SMC_ROM_ADDR_FILT_پیل   /* ? */
#تعریف_کړه MSHIELD_SMC_ROM_ADDR_FILT_پای_ټکی     /* ? */
/* MSHIELD Control 2 */
#تعریف_کړه MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#تعریف_کړه MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#تعریف_کړه MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#تعریف_کړه MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#تعریف_کړه PL310_LOCKDOWN_BY_ماسک            (0xf<<25)
#تعریف_کړه PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#تعریف_کړه PL310_LOCKDOWN_BY_کرښه            (0xf<<25)

/* Primary control */
#تعریف_کړه CTRL_CTRL_EN بټ(0)

/* Auxiliary control */
#تعریف_کړه CTRL_AUX_EARLY_BRESP_EN                            بټ(30)
#تعریف_کړه CTRL_AUX_IPREFETCH_EN                              بټ(29)
#تعریف_کړه CTRL_AUX_DPREFETCH_EN                              بټ(28)
#تعریف_کړه CTRL_AUX_NSECURE_INT_ACCESS                        بټ(27)
#تعریف_کړه CTRL_AUX_NSECURE_LOCKDOWN_EN                       بټ(26)
#تعریف_کړه CTRL_AUX_REPLACEMENT_POLICY                        بټ(25)
#تعریف_کړه CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * بټ(23)
#تعریف_کړه CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 بټ(22)
#تعریف_کړه CTRL_AUX_PARITY_EN                                 بټ(21)
#تعریف_کړه CTRL_AUX_EVENT_MONITOR_BUS_EN                      بټ(20)
#تعریف_کړه CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * بټ(17) )
#تعریف_کړه CTRL_AUX_ASSOCIATIVITY                             بټ(16)
#تعریف_کړه CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               بټ(13)
#تعریف_کړه CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    بټ(12)
#تعریف_کړه CTRL_AUX_STOREBUFDEV_حد_EN                      بټ(11)
#تعریف_کړه CTRL_AUX_لوړ_PRIO_SODEV_EN                        بټ(10)
#تعریف_کړه CTRL_AUX_ډک_کرښه_ZEROS_ENABLE                    بټ( 0)

#تعریف_کړه CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#تعریف_کړه CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#تعریف_کړه CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#تعریف_کړه CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#تعریف_کړه CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#تعریف_کړه CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#تعریف_کړه CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#تعریف_کړه CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#تعریف_کړه CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#تعریف_کړه CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#تعریف_کړه CTRL_RAM_LATENCY_سېټ(X,S)    (((X)&0x7) * بټ(S))
#تعریف_کړه CTRL_RAM_LATENCY_تیاری(X)    CTRL_RAM_LATENCY_سېټ(X, 0)
#تعریف_کړه CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_سېټ(X, 4)
#تعریف_کړه CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_سېټ(X, 8)

#تعریف_کړه CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_تیاری(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#تعریف_کړه MAINTENANCE_PENDING          بټ(0)

/* POWER */
#تعریف_کړه CTRL2_PWR_DYNAMIC_CLK_EN     بټ(1)
#تعریف_کړه CTRL2_PWR_STANDBY_ON         بټ(0)

/* PREFECTCH */
#تعریف_کړه CTRL2_PFET_DBL_LINEFILL_EN              بټ(30)
#تعریف_کړه CTRL2_PFET_INST_PREFETCH_EN             بټ(29)
#تعریف_کړه CTRL2_PFET_معلومات_PREFETCH_EN             بټ(28)
#تعریف_کړه CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      بټ(27)
#تعریف_کړه CTRL2_PFET_PREFETCH_DROP_EN             بټ(24)
#تعریف_کړه CTRL2_PFET_INCR_DBL_LINEFILL_EN         بټ(23)
#تعریف_کړه CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   بټ(21)
#تعریف_کړه CTRL2_PFET_PREFETCH_دننه_واټن(X)    ((X) * بټ( 0) )


بنية l2cc_map {

    بنية {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    بنية {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t نښان_ram_control;       /* 0x108 */
        uint32_t معلومات_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    بنية {
        uint32_t ev_counter_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_ماسک;              /* 0x214 */
        uint32_t int_ماسک_حالت;       /* 0x218 */
        uint32_t int_raw_حالت;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } مداخله /* reg2 */;

    بنية {
        uint32_t res[64];
    } reg3;
    بنية {
        uint32_t res[64];
    } reg4;
    بنية {
        uint32_t res[64];
    } reg5;
    بنية {
        uint32_t res[64];
    } reg6;

    بنية {
        uint32_t res[12];
        uint32_t cache_sync;            /* 0x730 */
        uint32_t res1[15];
        uint32_t inv_pa;                /* 0x770 */
        uint32_t res2[2];
        uint32_t inv_way;               /* 0x77C */
        uint32_t res3[12];
        uint32_t clean_pa;              /* 0x7B0 */
        uint32_t res4[1];
        uint32_t clean_شاخص;           /* 0x7B8 */
        uint32_t clean_way;             /* 0x7BC */
        uint32_t res5[12];
        uint32_t clean_inv_pa;          /* 0x7F0 */
        uint32_t res6[1];
        uint32_t clean_inv_شاخص;       /* 0x7F8 */
        uint32_t clean_inv_way;         /* 0x7FC */
    } maintenance /* reg7 */;

    بنية {
        uint32_t res[64];
    } reg8;

    بنية {
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
        uint32_t قلف_کرښه_eng;         /* 0x950 */
        uint32_t unlock_wayg;           /* 0x954 */
        uint32_t res1[42];
    } lockdown /* reg9 */;

    بنية {
        uint32_t res[64];
    } reg10;
    بنية {
        uint32_t res[64];
    } reg11;

    بنية {
        uint32_t addr_filtering_پیل;  /* 0xC00 */
        uint32_t addr_filtering_پای_ټکی;    /* 0xC04 */
        uint32_t res[62];
    } filter /* reg12 */;

    بنية {
        uint32_t res[64];
    } reg13;
    بنية {
        uint32_t res[64];
    } reg14;

    بنية {
        uint32_t res[16];
        uint32_t debug_ctrl;            /* 0xF40 */
        uint32_t res1[7];
        uint32_t prefetch_ctrl;         /* 0xF60 */
        uint32_t res2[7];
        uint32_t power_ctrl;            /* 0xF80 */
        uint32_t res3[31];
    } control2 /* reg15 */;
};


#که_نه_تعریف_شوی L2CC_L2C310_PPTR
#تېروتنه L2CC_L2C310_PPTR must be defined لكل مجازي حافظه access منزل the L2 cache controller
#وإلا  /* L2CC_PPTR */
متطاير بنية l2cc_map *ثابت l2cc
    = (متطاير بنية l2cc_map *)L2CC_L2C310_PPTR;
#د_که_پای /* !L2CC_PPTR */


#که_تعریف_شوی TI_MSHIELD
BOOT_کوډ ساكن فراغ mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
{
    سجل uint32_t _arg1 asm("r0") = arg1;
    سجل uint32_t _arg2 asm("r1") = arg2;
    سجل uint32_t _callid asm("r12") = callid;
    asm متطاير("push {r2-r12, lr}\n"
                 "dsb\n"
                 "smc #0\n"
                 "pop {r2-r12, lr}"
                 :: "r"(_callid), "r"(_arg1), "r"(_arg2));
}
#د_که_پای /* TI_MSHIELD */

BOOT_کوډ فراغ initL2Cache(فراغ)
{
#که_نه_تعریف_شوی CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t نښان_ram;
    uint32_t معلومات_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#که_نه_تعریف_شوی TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#د_که_پای

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_معلومات_PREFETCH_EN;
#إذا defined(CONFIG_PLAT_IMX6)
    نښان_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    معلومات_ram = CTRL_RAM_LATENCY(1, 2, 1);
#وإلا
    نښان_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    معلومات_ram = CTRL_RAM_LATENCY(1, 2, 0);
#د_که_پای

    aux      = 0
               | CTRL_AUX_IPREFETCH_EN
               | CTRL_AUX_DPREFETCH_EN
               | CTRL_AUX_NSECURE_INT_ACCESS
               | CTRL_AUX_NSECURE_LOCKDOWN_EN
               | CTRL_AUX_ASSOCIATIVITY_16WAY
               | CTRL_AUS_REPLPOLICY_RROBIN;

#إذا defined(CONFIG_PLAT_IMX6SX)
    aux |= CTRL_AUX_WAYSIZE_16K;
#وإلاإذا defined(CONFIG_PLAT_EXYNOS4) || defined(CONFIG_PLAT_IMX6) || defined(CONFIG_PLAT_ZYNQ7000) || defined(CONFIG_PLAT_ALLWINNERA20)
    aux |= CTRL_AUX_WAYSIZE_64K;
#وإلاإذا defined(OMAP4)
    aux |= CTRL_AUX_WAYSIZE_32K;
#وإلا /* ! (EXYNOS4 || OMAP4 || IMX6) */
#تېروتنه Unknown platform لكل L2C-310
#د_که_پای /* EXYNOS4 || OMAP4 || IMX6 */

#که_تعریف_شوی TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, نښان_ram, معلومات_ram);

#وإلا /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.نښان_ram_control  = نښان_ram;
    l2cc->control.معلومات_ram_control = معلومات_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#د_که_پای /* TI_MSHIELD */

    /* 2: Invalidate by way. */
    l2cc->maintenance.inv_way = 0xffff;
    طالما (l2cc->maintenance.inv_way & 0xffff);

    /* 3: write to lockdown D & I reg9 if required  */
    إذا ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ماسک) == PL310_LOCKDOWN_BY_MASTER) {
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
    إذا ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ماسک) == PL310_LOCKDOWN_BY_کرښه) {
        /* disable lockdown */
        l2cc->lockdown.قلف_کرښه_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->مداخله.int_ماسک  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->مداخله.int_clear = ماسک(9);

    /* 6: Enable the L2 cache */
#که_تعریف_شوی TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#وإلا /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#د_که_پای /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#إذا defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    سېټ_ACTLR(getACTLR() | بټ(1));
#د_که_پای

#د_که_پای /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ساكن مضمن فراغ L2_cacheSync(فراغ)
{
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    l2cc->maintenance.cache_sync = 0;
    طالما (l2cc->maintenance.cache_sync & MAINTENANCE_PENDING);
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
فراغ plat_cleanInvalidateL2Cache(فراغ)
{
    إذا (!config_سېټ(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        طالما (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

فراغ plat_cleanCache(فراغ)
{
#که_نه_تعریف_شوی CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    طالما (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#د_که_پای /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_cleanL2Range(paddr_t پیل, paddr_t پای_ټکی)
{
#که_نه_تعریف_شوی CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    لكل (پیل = L2_کرښه_پیل(پیل);
         پیل != L2_کرښه_پیل(پای_ټکی + L2_کرښه_اندازه);
         پیل += L2_کرښه_اندازه) {
        l2cc->maintenance.clean_pa = پیل;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#د_که_پای /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_invalidateL2Range(paddr_t پیل, paddr_t پای_ټکی)
{
#که_نه_تعریف_شوی CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    لكل (پیل = L2_کرښه_پیل(پیل);
         پیل != L2_کرښه_پیل(پای_ټکی + L2_کرښه_اندازه);
         پیل += L2_کرښه_اندازه) {
        l2cc->maintenance.inv_pa = پیل;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#د_که_پای /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_cleanInvalidateL2Range(paddr_t پیل, paddr_t پای_ټکی)
{
#که_نه_تعریف_شوی CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    لكل (پیل = L2_کرښه_پیل(پیل);
         پیل != L2_کرښه_پیل(پای_ټکی + L2_کرښه_اندازه);
         پیل += L2_کرښه_اندازه) {
        l2cc->maintenance.clean_inv_pa = پیل;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#د_که_پای /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

