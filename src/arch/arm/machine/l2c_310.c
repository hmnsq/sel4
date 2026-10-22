/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/l2c_310.h>

#تعریف_کن L2_سطر_اندازه_بیتها 5
#تعریف_کن L2_سطر_اندازه بیت(L2_سطر_اندازه_بیتها) /* 32 byte line size */

#تعریف_کن L2_سطر_شروع(a) دور_پایین(a, L2_سطر_اندازه_بیتها)

compile_assert(l2_l1_same_سطر_اندازه, L2_سطر_اندازه_بیتها == L1_CACHE_سطر_اندازه_بیتها)

/* MSHIELD Control */
#تعریف_کن MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#تعریف_کن MSHIELD_SMC_ROM_CTRL_AUX          0x109
#تعریف_کن MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#تعریف_کن MSHIELD_SMC_ROM_ADDR_FILT_شروع   /* ? */
#تعریف_کن MSHIELD_SMC_ROM_ADDR_FILT_انتها     /* ? */
/* MSHIELD Control 2 */
#تعریف_کن MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#تعریف_کن MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#تعریف_کن MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#تعریف_کن MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#تعریف_کن PL310_LOCKDOWN_BY_ماسک            (0xf<<25)
#تعریف_کن PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#تعریف_کن PL310_LOCKDOWN_BY_سطر            (0xf<<25)

/* Primary control */
#تعریف_کن CTRL_CTRL_EN بیت(0)

/* Auxiliary control */
#تعریف_کن CTRL_AUX_EARLY_BRESP_EN                            بیت(30)
#تعریف_کن CTRL_AUX_IPREFETCH_EN                              بیت(29)
#تعریف_کن CTRL_AUX_DPREFETCH_EN                              بیت(28)
#تعریف_کن CTRL_AUX_NSECURE_INT_ACCESS                        بیت(27)
#تعریف_کن CTRL_AUX_NSECURE_LOCKDOWN_EN                       بیت(26)
#تعریف_کن CTRL_AUX_REPLACEMENT_POLICY                        بیت(25)
#تعریف_کن CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * بیت(23)
#تعریف_کن CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 بیت(22)
#تعریف_کن CTRL_AUX_PARITY_EN                                 بیت(21)
#تعریف_کن CTRL_AUX_EVENT_MONITOR_BUS_EN                      بیت(20)
#تعریف_کن CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * بیت(17) )
#تعریف_کن CTRL_AUX_ASSOCIATIVITY                             بیت(16)
#تعریف_کن CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               بیت(13)
#تعریف_کن CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    بیت(12)
#تعریف_کن CTRL_AUX_STOREBUFDEV_حد_EN                      بیت(11)
#تعریف_کن CTRL_AUX_بالا_PRIO_SODEV_EN                        بیت(10)
#تعریف_کن CTRL_AUX_پر_سطر_ZEROS_ENABLE                    بیت( 0)

#تعریف_کن CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#تعریف_کن CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#تعریف_کن CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#تعریف_کن CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#تعریف_کن CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#تعریف_کن CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#تعریف_کن CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#تعریف_کن CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#تعریف_کن CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#تعریف_کن CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#تعریف_کن CTRL_RAM_LATENCY_مجموعه(X,S)    (((X)&0x7) * بیت(S))
#تعریف_کن CTRL_RAM_LATENCY_راهاندازی(X)    CTRL_RAM_LATENCY_مجموعه(X, 0)
#تعریف_کن CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_مجموعه(X, 4)
#تعریف_کن CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_مجموعه(X, 8)

#تعریف_کن CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_راهاندازی(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#تعریف_کن MAINTENANCE_PENDING          بیت(0)

/* POWER */
#تعریف_کن CTRL2_PWR_DYNAMIC_CLK_EN     بیت(1)
#تعریف_کن CTRL2_PWR_STANDBY_ON         بیت(0)

/* PREFECTCH */
#تعریف_کن CTRL2_PFET_DBL_LINEFILL_EN              بیت(30)
#تعریف_کن CTRL2_PFET_INST_PREFETCH_EN             بیت(29)
#تعریف_کن CTRL2_PFET_داده_PREFETCH_EN             بیت(28)
#تعریف_کن CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      بیت(27)
#تعریف_کن CTRL2_PFET_PREFETCH_DROP_EN             بیت(24)
#تعریف_کن CTRL2_PFET_INCR_DBL_LINEFILL_EN         بیت(23)
#تعریف_کن CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   بیت(21)
#تعریف_کن CTRL2_PFET_PREFETCH_جابه_جایی(X)    ((X) * بیت( 0) )


بنية l2cc_map {

    بنية {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    بنية {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t برچسب_ram_control;       /* 0x108 */
        uint32_t داده_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    بنية {
        uint32_t ev_counter_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_ماسک;              /* 0x214 */
        uint32_t int_ماسک_وضعیت;       /* 0x218 */
        uint32_t int_raw_وضعیت;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } وقفه /* reg2 */;

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
        uint32_t قفل_سطر_eng;         /* 0x950 */
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
        uint32_t addr_filtering_شروع;  /* 0xC00 */
        uint32_t addr_filtering_انتها;    /* 0xC04 */
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


#اگر_تعریف_نشده L2CC_L2C310_PPTR
#خطا L2CC_L2C310_PPTR must be defined لكل مجازی حافظه access مقصد the L2 cache controller
#وإلا  /* L2CC_PPTR */
متطاير بنية l2cc_map *ثابت l2cc
    = (متطاير بنية l2cc_map *)L2CC_L2C310_PPTR;
#پایان_اگر /* !L2CC_PPTR */


#اگر_تعریف_شده TI_MSHIELD
BOOT_کد ساكن فراغ mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
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
#پایان_اگر /* TI_MSHIELD */

BOOT_کد فراغ initL2Cache(فراغ)
{
#اگر_تعریف_نشده CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t برچسب_ram;
    uint32_t داده_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#اگر_تعریف_نشده TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#پایان_اگر

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_داده_PREFETCH_EN;
#إذا defined(CONFIG_PLAT_IMX6)
    برچسب_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    داده_ram = CTRL_RAM_LATENCY(1, 2, 1);
#وإلا
    برچسب_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    داده_ram = CTRL_RAM_LATENCY(1, 2, 0);
#پایان_اگر

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
#خطا Unknown platform لكل L2C-310
#پایان_اگر /* EXYNOS4 || OMAP4 || IMX6 */

#اگر_تعریف_شده TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, برچسب_ram, داده_ram);

#وإلا /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.برچسب_ram_control  = برچسب_ram;
    l2cc->control.داده_ram_control = داده_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#پایان_اگر /* TI_MSHIELD */

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
    إذا ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ماسک) == PL310_LOCKDOWN_BY_سطر) {
        /* disable lockdown */
        l2cc->lockdown.قفل_سطر_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->وقفه.int_ماسک  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->وقفه.int_clear = ماسک(9);

    /* 6: Enable the L2 cache */
#اگر_تعریف_شده TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#وإلا /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#پایان_اگر /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#إذا defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    مجموعه_ACTLR(getACTLR() | بیت(1));
#پایان_اگر

#پایان_اگر /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
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
    إذا (!config_مجموعه(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        طالما (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

فراغ plat_cleanCache(فراغ)
{
#اگر_تعریف_نشده CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    طالما (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#پایان_اگر /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_cleanL2Range(paddr_t شروع, paddr_t انتها)
{
#اگر_تعریف_نشده CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    لكل (شروع = L2_سطر_شروع(شروع);
         شروع != L2_سطر_شروع(انتها + L2_سطر_اندازه);
         شروع += L2_سطر_اندازه) {
        l2cc->maintenance.clean_pa = شروع;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#پایان_اگر /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_invalidateL2Range(paddr_t شروع, paddr_t انتها)
{
#اگر_تعریف_نشده CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    لكل (شروع = L2_سطر_شروع(شروع);
         شروع != L2_سطر_شروع(انتها + L2_سطر_اندازه);
         شروع += L2_سطر_اندازه) {
        l2cc->maintenance.inv_pa = شروع;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#پایان_اگر /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

فراغ plat_cleanInvalidateL2Range(paddr_t شروع, paddr_t انتها)
{
#اگر_تعریف_نشده CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    أكد(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    لكل (شروع = L2_سطر_شروع(شروع);
         شروع != L2_سطر_شروع(انتها + L2_سطر_اندازه);
         شروع += L2_سطر_اندازه) {
        l2cc->maintenance.clean_inv_pa = شروع;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#پایان_اگر /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

