/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/l2c_310.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ L2_ਸਤਰ_ਮਾਪ_ਬਿੱਟ 5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ L2_ਸਤਰ_ਮਾਪ ਦੋਆਧਾਰੀ_ਅੰਕ(L2_ਸਤਰ_ਮਾਪ_ਬਿੱਟ) /* 32 byte line size */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ L2_ਸਤਰ_ਸ਼ੁਰੂ(a) ਗੇੜਾ_ਥੱਲੇ(a, L2_ਸਤਰ_ਮਾਪ_ਬਿੱਟ)

compile_assert(l2_l1_same_ਸਤਰ_ਮਾਪ, L2_ਸਤਰ_ਮਾਪ_ਬਿੱਟ == L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ)

/* MSHIELD Control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_CTRL_AUX          0x109
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_ADDR_FILT_ਸ਼ੁਰੂ   /* ? */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_ADDR_FILT_ਸਿਰਾ     /* ? */
/* MSHIELD Control 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PL310_LOCKDOWN_BY_ਨਕਾਬ            (0xf<<25)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PL310_LOCKDOWN_BY_ਸਤਰ            (0xf<<25)

/* Primary control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_CTRL_EN ਦੋਆਧਾਰੀ_ਅੰਕ(0)

/* Auxiliary control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_EARLY_BRESP_EN                            ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_IPREFETCH_EN                              ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_DPREFETCH_EN                              ਦੋਆਧਾਰੀ_ਅੰਕ(28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_NSECURE_INT_ACCESS                        ਦੋਆਧਾਰੀ_ਅੰਕ(27)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_NSECURE_LOCKDOWN_EN                       ਦੋਆਧਾਰੀ_ਅੰਕ(26)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_REPLACEMENT_POLICY                        ਦੋਆਧਾਰੀ_ਅੰਕ(25)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * ਦੋਆਧਾਰੀ_ਅੰਕ(23)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 ਦੋਆਧਾਰੀ_ਅੰਕ(22)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_PARITY_EN                                 ਦੋਆਧਾਰੀ_ਅੰਕ(21)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_EVENT_MONITOR_BUS_EN                      ਦੋਆਧਾਰੀ_ਅੰਕ(20)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * ਦੋਆਧਾਰੀ_ਅੰਕ(17) )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_ASSOCIATIVITY                             ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               ਦੋਆਧਾਰੀ_ਅੰਕ(13)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    ਦੋਆਧਾਰੀ_ਅੰਕ(12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_STOREBUFDEV_ਹੱਦਬੰਦੀ_EN                      ਦੋਆਧਾਰੀ_ਅੰਕ(11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_ਉੱਚਾ_PRIO_SODEV_EN                        ਦੋਆਧਾਰੀ_ਅੰਕ(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_ਭਰਿਆ_ਸਤਰ_ZEROS_ENABLE                    ਦੋਆਧਾਰੀ_ਅੰਕ( 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_RAM_LATENCY_ਸੈੱਟ(X,S)    (((X)&0x7) * ਦੋਆਧਾਰੀ_ਅੰਕ(S))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_RAM_LATENCY_ਤਿਆਰੀ(X)    CTRL_RAM_LATENCY_ਸੈੱਟ(X, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_ਸੈੱਟ(X, 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_ਸੈੱਟ(X, 8)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_ਤਿਆਰੀ(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAINTENANCE_PENDING          ਦੋਆਧਾਰੀ_ਅੰਕ(0)

/* POWER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PWR_DYNAMIC_CLK_EN     ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PWR_STANDBY_ON         ਦੋਆਧਾਰੀ_ਅੰਕ(0)

/* PREFECTCH */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_DBL_LINEFILL_EN              ਦੋਆਧਾਰੀ_ਅੰਕ(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_INST_PREFETCH_EN             ਦੋਆਧਾਰੀ_ਅੰਕ(29)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_ਅੰਕੜੇ_PREFETCH_EN             ਦੋਆਧਾਰੀ_ਅੰਕ(28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      ਦੋਆਧਾਰੀ_ਅੰਕ(27)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_PREFETCH_DROP_EN             ਦੋਆਧਾਰੀ_ਅੰਕ(24)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_INCR_DBL_LINEFILL_EN         ਦੋਆਧਾਰੀ_ਅੰਕ(23)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   ਦੋਆਧਾਰੀ_ਅੰਕ(21)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL2_PFET_PREFETCH_ਵਿੱਥ(X)    ((X) * ਦੋਆਧਾਰੀ_ਅੰਕ( 0) )


ਬਣਤਰ l2cc_map {

    ਬਣਤਰ {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    ਬਣਤਰ {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t ਟੈਗ_ram_control;       /* 0x108 */
        uint32_t ਅੰਕੜੇ_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    ਬਣਤਰ {
        uint32_t ev_counter_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_ਨਕਾਬ;              /* 0x214 */
        uint32_t int_ਨਕਾਬ_ਸਥਿਤੀ;       /* 0x218 */
        uint32_t int_raw_ਸਥਿਤੀ;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } ਰੁਕਾਵਟ /* reg2 */;

    ਬਣਤਰ {
        uint32_t res[64];
    } reg3;
    ਬਣਤਰ {
        uint32_t res[64];
    } reg4;
    ਬਣਤਰ {
        uint32_t res[64];
    } reg5;
    ਬਣਤਰ {
        uint32_t res[64];
    } reg6;

    ਬਣਤਰ {
        uint32_t res[12];
        uint32_t cache_sync;            /* 0x730 */
        uint32_t res1[15];
        uint32_t inv_pa;                /* 0x770 */
        uint32_t res2[2];
        uint32_t inv_way;               /* 0x77C */
        uint32_t res3[12];
        uint32_t clean_pa;              /* 0x7B0 */
        uint32_t res4[1];
        uint32_t clean_ਸੂਚਕਾਂਕ;           /* 0x7B8 */
        uint32_t clean_way;             /* 0x7BC */
        uint32_t res5[12];
        uint32_t clean_inv_pa;          /* 0x7F0 */
        uint32_t res6[1];
        uint32_t clean_inv_ਸੂਚਕਾਂਕ;       /* 0x7F8 */
        uint32_t clean_inv_way;         /* 0x7FC */
    } maintenance /* reg7 */;

    ਬਣਤਰ {
        uint32_t res[64];
    } reg8;

    ਬਣਤਰ {
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
        uint32_t ਤਾਲਾ_ਸਤਰ_eng;         /* 0x950 */
        uint32_t unlock_wayg;           /* 0x954 */
        uint32_t res1[42];
    } lockdown /* reg9 */;

    ਬਣਤਰ {
        uint32_t res[64];
    } reg10;
    ਬਣਤਰ {
        uint32_t res[64];
    } reg11;

    ਬਣਤਰ {
        uint32_t addr_filtering_ਸ਼ੁਰੂ;  /* 0xC00 */
        uint32_t addr_filtering_ਸਿਰਾ;    /* 0xC04 */
        uint32_t res[62];
    } filter /* reg12 */;

    ਬਣਤਰ {
        uint32_t res[64];
    } reg13;
    ਬਣਤਰ {
        uint32_t res[64];
    } reg14;

    ਬਣਤਰ {
        uint32_t res[16];
        uint32_t debug_ctrl;            /* 0xF40 */
        uint32_t res1[7];
        uint32_t prefetch_ctrl;         /* 0xF60 */
        uint32_t res2[7];
        uint32_t power_ctrl;            /* 0xF80 */
        uint32_t res3[31];
    } control2 /* reg15 */;
};


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ L2CC_L2C310_PPTR
#ਗਲਤੀ L2CC_L2C310_PPTR must be defined ਲਈ ਆਭਾਸੀ ਸਮ੍ਰਿਤੀ access ਮੰਜ਼ਿਲ the L2 cache controller
#ਨਹੀਂਤਾਂ  /* L2CC_PPTR */
ਅਸਥਿਰ ਬਣਤਰ l2cc_map *ਅਚਲ l2cc
    = (ਅਸਥਿਰ ਬਣਤਰ l2cc_map *)L2CC_L2C310_PPTR;
#ਜੇ_ਅੰਤ /* !L2CC_PPTR */


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TI_MSHIELD
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
{
    ਰਜਿਸਟਰ uint32_t _arg1 asm("r0") = arg1;
    ਰਜਿਸਟਰ uint32_t _arg2 asm("r1") = arg2;
    ਰਜਿਸਟਰ uint32_t _callid asm("r12") = callid;
    asm ਅਸਥਿਰ("push {r2-r12, lr}\n"
                 "dsb\n"
                 "smc #0\n"
                 "pop {r2-r12, lr}"
                 :: "r"(_callid), "r"(_arg1), "r"(_arg2));
}
#ਜੇ_ਅੰਤ /* TI_MSHIELD */

BOOT_ਕੋਡ ਖਾਲੀ initL2Cache(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t ਟੈਗ_ram;
    uint32_t ਅੰਕੜੇ_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#ਜੇ_ਅੰਤ

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_ਅੰਕੜੇ_PREFETCH_EN;
#ਜੇ defined(CONFIG_PLAT_IMX6)
    ਟੈਗ_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    ਅੰਕੜੇ_ram = CTRL_RAM_LATENCY(1, 2, 1);
#ਨਹੀਂਤਾਂ
    ਟੈਗ_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    ਅੰਕੜੇ_ram = CTRL_RAM_LATENCY(1, 2, 0);
#ਜੇ_ਅੰਤ

    aux      = 0
               | CTRL_AUX_IPREFETCH_EN
               | CTRL_AUX_DPREFETCH_EN
               | CTRL_AUX_NSECURE_INT_ACCESS
               | CTRL_AUX_NSECURE_LOCKDOWN_EN
               | CTRL_AUX_ASSOCIATIVITY_16WAY
               | CTRL_AUS_REPLPOLICY_RROBIN;

#ਜੇ defined(CONFIG_PLAT_IMX6SX)
    aux |= CTRL_AUX_WAYSIZE_16K;
#ਜਾਂਜੇ defined(CONFIG_PLAT_EXYNOS4) || defined(CONFIG_PLAT_IMX6) || defined(CONFIG_PLAT_ZYNQ7000) || defined(CONFIG_PLAT_ALLWINNERA20)
    aux |= CTRL_AUX_WAYSIZE_64K;
#ਜਾਂਜੇ defined(OMAP4)
    aux |= CTRL_AUX_WAYSIZE_32K;
#ਨਹੀਂਤਾਂ /* ! (EXYNOS4 || OMAP4 || IMX6) */
#ਗਲਤੀ Unknown platform ਲਈ L2C-310
#ਜੇ_ਅੰਤ /* EXYNOS4 || OMAP4 || IMX6 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, ਟੈਗ_ram, ਅੰਕੜੇ_ram);

#ਨਹੀਂਤਾਂ /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.ਟੈਗ_ram_control  = ਟੈਗ_ram;
    l2cc->control.ਅੰਕੜੇ_ram_control = ਅੰਕੜੇ_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#ਜੇ_ਅੰਤ /* TI_MSHIELD */

    /* 2: Invalidate by way. */
    l2cc->maintenance.inv_way = 0xffff;
    ਜਦੋਂ (l2cc->maintenance.inv_way & 0xffff);

    /* 3: write to lockdown D & I reg9 if required  */
    ਜੇ ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ਨਕਾਬ) == PL310_LOCKDOWN_BY_MASTER) {
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
    ਜੇ ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ਨਕਾਬ) == PL310_LOCKDOWN_BY_ਸਤਰ) {
        /* disable lockdown */
        l2cc->lockdown.ਤਾਲਾ_ਸਤਰ_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->ਰੁਕਾਵਟ.int_ਨਕਾਬ  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->ਰੁਕਾਵਟ.int_clear = ਨਕਾਬ(9);

    /* 6: Enable the L2 cache */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#ਨਹੀਂਤਾਂ /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#ਜੇ_ਅੰਤ /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#ਜੇ defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    ਸੈੱਟ_ACTLR(getACTLR() | ਦੋਆਧਾਰੀ_ਅੰਕ(1));
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ L2_cacheSync(ਖਾਲੀ)
{
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    l2cc->maintenance.cache_sync = 0;
    ਜਦੋਂ (l2cc->maintenance.cache_sync & MAINTENANCE_PENDING);
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
ਖਾਲੀ plat_cleanInvalidateL2Cache(ਖਾਲੀ)
{
    ਜੇ (!config_ਸੈੱਟ(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        ਜਦੋਂ (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

ਖਾਲੀ plat_cleanCache(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    ਜਦੋਂ (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#ਜੇ_ਅੰਤ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ਖਾਲੀ plat_cleanL2Range(paddr_t ਸ਼ੁਰੂ, paddr_t ਸਿਰਾ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ਪ੍ਰਤਿਜ੍ਞਾ(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    ਲਈ (ਸ਼ੁਰੂ = L2_ਸਤਰ_ਸ਼ੁਰੂ(ਸ਼ੁਰੂ);
         ਸ਼ੁਰੂ != L2_ਸਤਰ_ਸ਼ੁਰੂ(ਸਿਰਾ + L2_ਸਤਰ_ਮਾਪ);
         ਸ਼ੁਰੂ += L2_ਸਤਰ_ਮਾਪ) {
        l2cc->maintenance.clean_pa = ਸ਼ੁਰੂ;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#ਜੇ_ਅੰਤ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ਖਾਲੀ plat_invalidateL2Range(paddr_t ਸ਼ੁਰੂ, paddr_t ਸਿਰਾ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ਪ੍ਰਤਿਜ੍ਞਾ(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    ਲਈ (ਸ਼ੁਰੂ = L2_ਸਤਰ_ਸ਼ੁਰੂ(ਸ਼ੁਰੂ);
         ਸ਼ੁਰੂ != L2_ਸਤਰ_ਸ਼ੁਰੂ(ਸਿਰਾ + L2_ਸਤਰ_ਮਾਪ);
         ਸ਼ੁਰੂ += L2_ਸਤਰ_ਮਾਪ) {
        l2cc->maintenance.inv_pa = ਸ਼ੁਰੂ;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#ਜੇ_ਅੰਤ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ਖਾਲੀ plat_cleanInvalidateL2Range(paddr_t ਸ਼ੁਰੂ, paddr_t ਸਿਰਾ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ਪ੍ਰਤਿਜ੍ਞਾ(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    ਲਈ (ਸ਼ੁਰੂ = L2_ਸਤਰ_ਸ਼ੁਰੂ(ਸ਼ੁਰੂ);
         ਸ਼ੁਰੂ != L2_ਸਤਰ_ਸ਼ੁਰੂ(ਸਿਰਾ + L2_ਸਤਰ_ਮਾਪ);
         ਸ਼ੁਰੂ += L2_ਸਤਰ_ਮਾਪ) {
        l2cc->maintenance.clean_inv_pa = ਸ਼ੁਰੂ;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#ਜੇ_ਅੰਤ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

