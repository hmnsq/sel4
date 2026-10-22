/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/l2c_310.h>

#وصف_ڪريو L2_سٽ_سائيز_بٽ 5
#وصف_ڪريو L2_سٽ_سائيز ٻٽو_انگ(L2_سٽ_سائيز_بٽ) /* 32 byte line size */

#وصف_ڪريو L2_سٽ_شروعات(a) ڦيرو_هيٺ_ڏانهن(a, L2_سٽ_سائيز_بٽ)

compile_assert(l2_l1_same_سٽ_سائيز, L2_سٽ_سائيز_بٽ == L1_CACHE_سٽ_سائيز_بٽ)

/* MSHIELD Control */
#وصف_ڪريو MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#وصف_ڪريو MSHIELD_SMC_ROM_CTRL_AUX          0x109
#وصف_ڪريو MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#وصف_ڪريو MSHIELD_SMC_ROM_ADDR_FILT_شروعات   /* ? */
#وصف_ڪريو MSHIELD_SMC_ROM_ADDR_FILT_پڇاڙي     /* ? */
/* MSHIELD Control 2 */
#وصف_ڪريو MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#وصف_ڪريو MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#وصف_ڪريو MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#وصف_ڪريو MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#وصف_ڪريو PL310_LOCKDOWN_BY_نقاب            (0xf<<25)
#وصف_ڪريو PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#وصف_ڪريو PL310_LOCKDOWN_BY_سٽ            (0xf<<25)

/* Primary control */
#وصف_ڪريو CTRL_CTRL_EN ٻٽو_انگ(0)

/* Auxiliary control */
#وصف_ڪريو CTRL_AUX_EARLY_BRESP_EN                            ٻٽو_انگ(30)
#وصف_ڪريو CTRL_AUX_IPREFETCH_EN                              ٻٽو_انگ(29)
#وصف_ڪريو CTRL_AUX_DPREFETCH_EN                              ٻٽو_انگ(28)
#وصف_ڪريو CTRL_AUX_NSECURE_INT_ACCESS                        ٻٽو_انگ(27)
#وصف_ڪريو CTRL_AUX_NSECURE_LOCKDOWN_EN                       ٻٽو_انگ(26)
#وصف_ڪريو CTRL_AUX_REPLACEMENT_POLICY                        ٻٽو_انگ(25)
#وصف_ڪريو CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * ٻٽو_انگ(23)
#وصف_ڪريو CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 ٻٽو_انگ(22)
#وصف_ڪريو CTRL_AUX_PARITY_EN                                 ٻٽو_انگ(21)
#وصف_ڪريو CTRL_AUX_EVENT_MONITOR_BUS_EN                      ٻٽو_انگ(20)
#وصف_ڪريو CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * ٻٽو_انگ(17) )
#وصف_ڪريو CTRL_AUX_ASSOCIATIVITY                             ٻٽو_انگ(16)
#وصف_ڪريو CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               ٻٽو_انگ(13)
#وصف_ڪريو CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    ٻٽو_انگ(12)
#وصف_ڪريو CTRL_AUX_STOREBUFDEV_حد_بندي_EN                      ٻٽو_انگ(11)
#وصف_ڪريو CTRL_AUX_مٿاهون_PRIO_SODEV_EN                        ٻٽو_انگ(10)
#وصف_ڪريو CTRL_AUX_ڀريل_سٽ_ZEROS_ENABLE                    ٻٽو_انگ( 0)

#وصف_ڪريو CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#وصف_ڪريو CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#وصف_ڪريو CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#وصف_ڪريو CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#وصف_ڪريو CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#وصف_ڪريو CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#وصف_ڪريو CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#وصف_ڪريو CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#وصف_ڪريو CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#وصف_ڪريو CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#وصف_ڪريو CTRL_RAM_LATENCY_سيٽ(X,S)    (((X)&0x7) * ٻٽو_انگ(S))
#وصف_ڪريو CTRL_RAM_LATENCY_تياري(X)    CTRL_RAM_LATENCY_سيٽ(X, 0)
#وصف_ڪريو CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_سيٽ(X, 4)
#وصف_ڪريو CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_سيٽ(X, 8)

#وصف_ڪريو CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_تياري(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#وصف_ڪريو MAINTENANCE_PENDING          ٻٽو_انگ(0)

/* POWER */
#وصف_ڪريو CTRL2_PWR_DYNAMIC_CLK_EN     ٻٽو_انگ(1)
#وصف_ڪريو CTRL2_PWR_STANDBY_ON         ٻٽو_انگ(0)

/* PREFECTCH */
#وصف_ڪريو CTRL2_PFET_DBL_LINEFILL_EN              ٻٽو_انگ(30)
#وصف_ڪريو CTRL2_PFET_INST_PREFETCH_EN             ٻٽو_انگ(29)
#وصف_ڪريو CTRL2_PFET_معلومات_PREFETCH_EN             ٻٽو_انگ(28)
#وصف_ڪريو CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      ٻٽو_انگ(27)
#وصف_ڪريو CTRL2_PFET_PREFETCH_DROP_EN             ٻٽو_انگ(24)
#وصف_ڪريو CTRL2_PFET_INCR_DBL_LINEFILL_EN         ٻٽو_انگ(23)
#وصف_ڪريو CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   ٻٽو_انگ(21)
#وصف_ڪريو CTRL2_PFET_PREFETCH_وٿي(X)    ((X) * ٻٽو_انگ( 0) )


بنية l2cc_map {

    بنية {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    بنية {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t ٽيگ_ram_control;       /* 0x108 */
        uint32_t معلومات_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    بنية {
        uint32_t ev_counter_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_نقاب;              /* 0x214 */
        uint32_t int_نقاب_حالت;       /* 0x218 */
        uint32_t int_raw_حالت;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } مداخلت /* reg2 */;

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
        uint32_t clean_اشاريو;           /* 0x7B8 */
        uint32_t clean_way;             /* 0x7BC */
        uint32_t res5[12];
        uint32_t clean_inv_pa;          /* 0x7F0 */
        uint32_t res6[1];
        uint32_t clean_inv_اشاريو;       /* 0x7F8 */
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
        uint32_t تالو_سٽ_eng;         /* 0x950 */
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
        uint32_t addr_filtering_شروعات;  /* 0xC00 */
        uint32_t addr_filtering_پڇاڙي;    /* 0xC04 */
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


#جيڪڏهن_وصف_نه_ٿيل L2CC_L2C310_PPTR
#غلطي L2CC_L2C310_PPTR must be defined لكل مجازي يادگيري access منزل the L2 cache controller
#وإلا  /* L2CC_PPTR */
متطاير بنية l2cc_map *ثابت l2cc
    = (متطاير بنية l2cc_map *)L2CC_L2C310_PPTR;
#جيڪڏهن_پورو /* !L2CC_PPTR */


#جيڪڏهن_وصف_ٿيل TI_MSHIELD
BOOT_ڪوڊ ساكن فراغ mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
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
#جيڪڏهن_پورو /* TI_MSHIELD */

BOOT_ڪوڊ فراغ initL2Cache(فراغ)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t ٽيگ_ram;
    uint32_t معلومات_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#جيڪڏهن_وصف_نه_ٿيل TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#جيڪڏهن_پورو

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_معلومات_PREFETCH_EN;
#إذا defined(CONFIG_PLAT_IMX6)
    ٽيگ_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    معلومات_ram = CTRL_RAM_LATENCY(1, 2, 1);
#وإلا
    ٽيگ_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    معلومات_ram = CTRL_RAM_LATENCY(1, 2, 0);
#جيڪڏهن_پورو

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
#غلطي Unknown platform لكل L2C-310
#جيڪڏهن_پورو /* EXYNOS4 || OMAP4 || IMX6 */

#جيڪڏهن_وصف_ٿيل TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, ٽيگ_ram, معلومات_ram);

#وإلا /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.ٽيگ_ram_control  = ٽيگ_ram;
    l2cc->control.معلومات_ram_control = معلومات_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#جيڪڏهن_پورو /* TI_MSHIELD */

    /* 2: Invalidate by way. */
    l2cc->maintenance.inv_way = 0xffff;
    طالما (l2cc->maintenance.inv_way & 0xffff);

    /* 3: write to lockdown D & I reg9 if required  */
    إذا ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_نقاب) == PL310_LOCKDOWN_BY_MASTER) {
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
    إذا ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_نقاب) == PL310_LOCKDOWN_BY_سٽ) {
        /* disable lockdown */
        l2cc->lockdown.تالو_سٽ_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->مداخلت.int_نقاب  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->مداخلت.int_clear = نقاب(9);

    /* 6: Enable the L2 cache */
#جيڪڏهن_وصف_ٿيل TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#وإلا /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#جيڪڏهن_پورو /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#إذا defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    سيٽ_ACTLR(getACTLR() | ٻٽو_انگ(1));
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
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
    إذا (!config_سيٽ(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        طالما (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

فراغ plat_cleanCache(فراغ)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    طالما (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#جيڪڏهن_پورو /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_cleanL2Range(paddr_t شروعات, paddr_t پڇاڙي)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    لكل (شروعات = L2_سٽ_شروعات(شروعات);
         شروعات != L2_سٽ_شروعات(پڇاڙي + L2_سٽ_سائيز);
         شروعات += L2_سٽ_سائيز) {
        l2cc->maintenance.clean_pa = شروعات;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#جيڪڏهن_پورو /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_invalidateL2Range(paddr_t شروعات, paddr_t پڇاڙي)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    لكل (شروعات = L2_سٽ_شروعات(شروعات);
         شروعات != L2_سٽ_شروعات(پڇاڙي + L2_سٽ_سائيز);
         شروعات += L2_سٽ_سائيز) {
        l2cc->maintenance.inv_pa = شروعات;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#جيڪڏهن_پورو /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_cleanInvalidateL2Range(paddr_t شروعات, paddr_t پڇاڙي)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    لكل (شروعات = L2_سٽ_شروعات(شروعات);
         شروعات != L2_سٽ_شروعات(پڇاڙي + L2_سٽ_سائيز);
         شروعات += L2_سٽ_سائيز) {
        l2cc->maintenance.clean_inv_pa = شروعات;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#جيڪڏهن_پورو /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

