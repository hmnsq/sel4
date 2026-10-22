/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/machine/l2c_310.h>

#నిర్వచించు L2_పంక్తి_కొలత_బిట్లు 5
#నిర్వచించు L2_పంక్తి_కొలత బిట్టు(L2_పంక్తి_కొలత_బిట్లు) /* 32 byte line size */

#నిర్వచించు L2_పంక్తి_ప్రారంభం(a) విడత_కిందికి(a, L2_పంక్తి_కొలత_బిట్లు)

compile_assert(l2_l1_same_పంక్తి_కొలత, L2_పంక్తి_కొలత_బిట్లు == L1_CACHE_పంక్తి_కొలత_బిట్లు)

/* MSHIELD Control */
#నిర్వచించు MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#నిర్వచించు MSHIELD_SMC_ROM_CTRL_AUX          0x109
#నిర్వచించు MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#నిర్వచించు MSHIELD_SMC_ROM_ADDR_FILT_ప్రారంభం   /* ? */
#నిర్వచించు MSHIELD_SMC_ROM_ADDR_FILT_చివర     /* ? */
/* MSHIELD Control 2 */
#నిర్వచించు MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#నిర్వచించు MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#నిర్వచించు MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#నిర్వచించు MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#నిర్వచించు PL310_LOCKDOWN_BY_ముసుగు            (0xf<<25)
#నిర్వచించు PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#నిర్వచించు PL310_LOCKDOWN_BY_పంక్తి            (0xf<<25)

/* Primary control */
#నిర్వచించు CTRL_CTRL_EN బిట్టు(0)

/* Auxiliary control */
#నిర్వచించు CTRL_AUX_EARLY_BRESP_EN                            బిట్టు(30)
#నిర్వచించు CTRL_AUX_IPREFETCH_EN                              బిట్టు(29)
#నిర్వచించు CTRL_AUX_DPREFETCH_EN                              బిట్టు(28)
#నిర్వచించు CTRL_AUX_NSECURE_INT_ACCESS                        బిట్టు(27)
#నిర్వచించు CTRL_AUX_NSECURE_LOCKDOWN_EN                       బిట్టు(26)
#నిర్వచించు CTRL_AUX_REPLACEMENT_POLICY                        బిట్టు(25)
#నిర్వచించు CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * బిట్టు(23)
#నిర్వచించు CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 బిట్టు(22)
#నిర్వచించు CTRL_AUX_PARITY_EN                                 బిట్టు(21)
#నిర్వచించు CTRL_AUX_EVENT_MONITOR_BUS_EN                      బిట్టు(20)
#నిర్వచించు CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * బిట్టు(17) )
#నిర్వచించు CTRL_AUX_ASSOCIATIVITY                             బిట్టు(16)
#నిర్వచించు CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               బిట్టు(13)
#నిర్వచించు CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    బిట్టు(12)
#నిర్వచించు CTRL_AUX_STOREBUFDEV_హద్దు_EN                      బిట్టు(11)
#నిర్వచించు CTRL_AUX_ఎగువ_PRIO_SODEV_EN                        బిట్టు(10)
#నిర్వచించు CTRL_AUX_నిండిన_పంక్తి_ZEROS_ENABLE                    బిట్టు( 0)

#నిర్వచించు CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#నిర్వచించు CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#నిర్వచించు CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#నిర్వచించు CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#నిర్వచించు CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#నిర్వచించు CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#నిర్వచించు CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#నిర్వచించు CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#నిర్వచించు CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#నిర్వచించు CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#నిర్వచించు CTRL_RAM_LATENCY_సమితి(X,S)    (((X)&0x7) * బిట్టు(S))
#నిర్వచించు CTRL_RAM_LATENCY_అమరిక(X)    CTRL_RAM_LATENCY_సమితి(X, 0)
#నిర్వచించు CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_సమితి(X, 4)
#నిర్వచించు CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_సమితి(X, 8)

#నిర్వచించు CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_అమరిక(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#నిర్వచించు MAINTENANCE_PENDING          బిట్టు(0)

/* POWER */
#నిర్వచించు CTRL2_PWR_DYNAMIC_CLK_EN     బిట్టు(1)
#నిర్వచించు CTRL2_PWR_STANDBY_ON         బిట్టు(0)

/* PREFECTCH */
#నిర్వచించు CTRL2_PFET_DBL_LINEFILL_EN              బిట్టు(30)
#నిర్వచించు CTRL2_PFET_INST_PREFETCH_EN             బిట్టు(29)
#నిర్వచించు CTRL2_PFET_దత్తాంశం_PREFETCH_EN             బిట్టు(28)
#నిర్వచించు CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      బిట్టు(27)
#నిర్వచించు CTRL2_PFET_PREFETCH_DROP_EN             బిట్టు(24)
#నిర్వచించు CTRL2_PFET_INCR_DBL_LINEFILL_EN         బిట్టు(23)
#నిర్వచించు CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   బిట్టు(21)
#నిర్వచించు CTRL2_PFET_PREFETCH_స్థానభ్రంశం(X)    ((X) * బిట్టు( 0) )


నిర్మాణం l2cc_map {

    నిర్మాణం {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    నిర్మాణం {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t ట్యాగ్_ram_control;       /* 0x108 */
        uint32_t దత్తాంశం_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    నిర్మాణం {
        uint32_t ev_counter_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_ముసుగు;              /* 0x214 */
        uint32_t int_ముసుగు_స్థితి;       /* 0x218 */
        uint32_t int_raw_స్థితి;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } అంతరాయం /* reg2 */;

    నిర్మాణం {
        uint32_t res[64];
    } reg3;
    నిర్మాణం {
        uint32_t res[64];
    } reg4;
    నిర్మాణం {
        uint32_t res[64];
    } reg5;
    నిర్మాణం {
        uint32_t res[64];
    } reg6;

    నిర్మాణం {
        uint32_t res[12];
        uint32_t cache_sync;            /* 0x730 */
        uint32_t res1[15];
        uint32_t inv_pa;                /* 0x770 */
        uint32_t res2[2];
        uint32_t inv_way;               /* 0x77C */
        uint32_t res3[12];
        uint32_t clean_pa;              /* 0x7B0 */
        uint32_t res4[1];
        uint32_t clean_సూచీ;           /* 0x7B8 */
        uint32_t clean_way;             /* 0x7BC */
        uint32_t res5[12];
        uint32_t clean_inv_pa;          /* 0x7F0 */
        uint32_t res6[1];
        uint32_t clean_inv_సూచీ;       /* 0x7F8 */
        uint32_t clean_inv_way;         /* 0x7FC */
    } maintenance /* reg7 */;

    నిర్మాణం {
        uint32_t res[64];
    } reg8;

    నిర్మాణం {
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
        uint32_t తాళం_పంక్తి_eng;         /* 0x950 */
        uint32_t unlock_wayg;           /* 0x954 */
        uint32_t res1[42];
    } lockdown /* reg9 */;

    నిర్మాణం {
        uint32_t res[64];
    } reg10;
    నిర్మాణం {
        uint32_t res[64];
    } reg11;

    నిర్మాణం {
        uint32_t addr_filtering_ప్రారంభం;  /* 0xC00 */
        uint32_t addr_filtering_చివర;    /* 0xC04 */
        uint32_t res[62];
    } filter /* reg12 */;

    నిర్మాణం {
        uint32_t res[64];
    } reg13;
    నిర్మాణం {
        uint32_t res[64];
    } reg14;

    నిర్మాణం {
        uint32_t res[16];
        uint32_t debug_ctrl;            /* 0xF40 */
        uint32_t res1[7];
        uint32_t prefetch_ctrl;         /* 0xF60 */
        uint32_t res2[7];
        uint32_t power_ctrl;            /* 0xF80 */
        uint32_t res3[31];
    } control2 /* reg15 */;
};


#నిర్వచితం_కాకపోతే L2CC_L2C310_PPTR
#దోషం L2CC_L2C310_PPTR must be defined కొరకు మాయా జ్ఞాపకం access గమ్యం the L2 cache controller
#లేకపోతే  /* L2CC_PPTR */
అస్థిర నిర్మాణం l2cc_map *స్థిరరాశి l2cc
    = (అస్థిర నిర్మాణం l2cc_map *)L2CC_L2C310_PPTR;
#అయితే_ముగింపు /* !L2CC_PPTR */


#నిర్వచితమైతే TI_MSHIELD
BOOT_సంకేతం స్థిర శూన్య mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
{
    నమోదు uint32_t _arg1 asm("r0") = arg1;
    నమోదు uint32_t _arg2 asm("r1") = arg2;
    నమోదు uint32_t _callid asm("r12") = callid;
    asm అస్థిర("push {r2-r12, lr}\n"
                 "dsb\n"
                 "smc #0\n"
                 "pop {r2-r12, lr}"
                 :: "r"(_callid), "r"(_arg1), "r"(_arg2));
}
#అయితే_ముగింపు /* TI_MSHIELD */

BOOT_సంకేతం శూన్య initL2Cache(శూన్య)
{
#నిర్వచితం_కాకపోతే CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t ట్యాగ్_ram;
    uint32_t దత్తాంశం_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#నిర్వచితం_కాకపోతే TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#అయితే_ముగింపు

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_దత్తాంశం_PREFETCH_EN;
#ఒకవేళ defined(CONFIG_PLAT_IMX6)
    ట్యాగ్_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    దత్తాంశం_ram = CTRL_RAM_LATENCY(1, 2, 1);
#లేకపోతే
    ట్యాగ్_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    దత్తాంశం_ram = CTRL_RAM_LATENCY(1, 2, 0);
#అయితే_ముగింపు

    aux      = 0
               | CTRL_AUX_IPREFETCH_EN
               | CTRL_AUX_DPREFETCH_EN
               | CTRL_AUX_NSECURE_INT_ACCESS
               | CTRL_AUX_NSECURE_LOCKDOWN_EN
               | CTRL_AUX_ASSOCIATIVITY_16WAY
               | CTRL_AUS_REPLPOLICY_RROBIN;

#ఒకవేళ defined(CONFIG_PLAT_IMX6SX)
    aux |= CTRL_AUX_WAYSIZE_16K;
#లేదావేళ defined(CONFIG_PLAT_EXYNOS4) || defined(CONFIG_PLAT_IMX6) || defined(CONFIG_PLAT_ZYNQ7000) || defined(CONFIG_PLAT_ALLWINNERA20)
    aux |= CTRL_AUX_WAYSIZE_64K;
#లేదావేళ defined(OMAP4)
    aux |= CTRL_AUX_WAYSIZE_32K;
#లేకపోతే /* ! (EXYNOS4 || OMAP4 || IMX6) */
#దోషం Unknown platform కొరకు L2C-310
#అయితే_ముగింపు /* EXYNOS4 || OMAP4 || IMX6 */

#నిర్వచితమైతే TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, ట్యాగ్_ram, దత్తాంశం_ram);

#లేకపోతే /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.ట్యాగ్_ram_control  = ట్యాగ్_ram;
    l2cc->control.దత్తాంశం_ram_control = దత్తాంశం_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#అయితే_ముగింపు /* TI_MSHIELD */

    /* 2: Invalidate by way. */
    l2cc->maintenance.inv_way = 0xffff;
    అయితే (l2cc->maintenance.inv_way & 0xffff);

    /* 3: write to lockdown D & I reg9 if required  */
    ఒకవేళ ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ముసుగు) == PL310_LOCKDOWN_BY_MASTER) {
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
    ఒకవేళ ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ముసుగు) == PL310_LOCKDOWN_BY_పంక్తి) {
        /* disable lockdown */
        l2cc->lockdown.తాళం_పంక్తి_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->అంతరాయం.int_ముసుగు  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->అంతరాయం.int_clear = ముసుగు(9);

    /* 6: Enable the L2 cache */
#నిర్వచితమైతే TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#లేకపోతే /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#అయితే_ముగింపు /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#ఒకవేళ defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    సమితి_ACTLR(getACTLR() | బిట్టు(1));
#అయితే_ముగింపు

#అయితే_ముగింపు /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

స్థిర అంతఃవరుస శూన్య L2_cacheSync(శూన్య)
{
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    l2cc->maintenance.cache_sync = 0;
    అయితే (l2cc->maintenance.cache_sync & MAINTENANCE_PENDING);
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
శూన్య plat_cleanInvalidateL2Cache(శూన్య)
{
    ఒకవేళ (!config_సమితి(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        అయితే (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

శూన్య plat_cleanCache(శూన్య)
{
#నిర్వచితం_కాకపోతే CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    అయితే (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#అయితే_ముగింపు /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

శూన్య plat_cleanL2Range(paddr_t ప్రారంభం, paddr_t చివర)
{
#నిర్వచితం_కాకపోతే CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ప్రతిజ్ఞా(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    కొరకు (ప్రారంభం = L2_పంక్తి_ప్రారంభం(ప్రారంభం);
         ప్రారంభం != L2_పంక్తి_ప్రారంభం(చివర + L2_పంక్తి_కొలత);
         ప్రారంభం += L2_పంక్తి_కొలత) {
        l2cc->maintenance.clean_pa = ప్రారంభం;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#అయితే_ముగింపు /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

శూన్య plat_invalidateL2Range(paddr_t ప్రారంభం, paddr_t చివర)
{
#నిర్వచితం_కాకపోతే CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ప్రతిజ్ఞా(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    కొరకు (ప్రారంభం = L2_పంక్తి_ప్రారంభం(ప్రారంభం);
         ప్రారంభం != L2_పంక్తి_ప్రారంభం(చివర + L2_పంక్తి_కొలత);
         ప్రారంభం += L2_పంక్తి_కొలత) {
        l2cc->maintenance.inv_pa = ప్రారంభం;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#అయితే_ముగింపు /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

శూన్య plat_cleanInvalidateL2Range(paddr_t ప్రారంభం, paddr_t చివర)
{
#నిర్వచితం_కాకపోతే CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ప్రతిజ్ఞా(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    కొరకు (ప్రారంభం = L2_పంక్తి_ప్రారంభం(ప్రారంభం);
         ప్రారంభం != L2_పంక్తి_ప్రారంభం(చివర + L2_పంక్తి_కొలత);
         ప్రారంభం += L2_పంక్తి_కొలత) {
        l2cc->maintenance.clean_inv_pa = ప్రారంభం;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#అయితే_ముగింపు /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

