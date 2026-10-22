/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2020, HENSOLDT Cyber GmbH
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/l2c_310.h>

#ವ್ಯಾಖ್ಯಾನಿಸು L2_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು 5
#ವ್ಯಾಖ್ಯಾನಿಸು L2_ಸಾಲು_ಗಾತ್ರ ಬಿಟ್(L2_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) /* 32 byte line size */

#ವ್ಯಾಖ್ಯಾನಿಸು L2_ಸಾಲು_ಆರಂಭ(a) ಸುತ್ತು_ಕೆಳಕ್ಕೆ(a, L2_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)

compile_assert(l2_l1_same_ಸಾಲು_ಗಾತ್ರ, L2_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು == L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)

/* MSHIELD Control */
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_CTRL_CTRL         0x102
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_CTRL_AUX          0x109
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_CTRL_LATENCY      0x112
/* MSHIELD Address Filter */
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_ADDR_FILT_ಆರಂಭ   /* ? */
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_ADDR_FILT_ಕೊನೆ     /* ? */
/* MSHIELD Control 2 */
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_CTRL2_DEBUG       0x100
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_CTRL2_PREFETCH    0x113 /* ? */
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_CTRL2_POWER       /* ? */
/* MSHIELD Cache maintenance */
#ವ್ಯಾಖ್ಯಾನಿಸು MSHIELD_SMC_ROM_MAINT_INVALIDATE  0x101


/* Cache ID */
#ವ್ಯಾಖ್ಯಾನಿಸು PL310_LOCKDOWN_BY_ಮುಖವಾಡ            (0xf<<25)
#ವ್ಯಾಖ್ಯಾನಿಸು PL310_LOCKDOWN_BY_MASTER          (0xe<<25)
#ವ್ಯಾಖ್ಯಾನಿಸು PL310_LOCKDOWN_BY_ಸಾಲು            (0xf<<25)

/* Primary control */
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_CTRL_EN ಬಿಟ್(0)

/* Auxiliary control */
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_EARLY_BRESP_EN                            ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_IPREFETCH_EN                              ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_DPREFETCH_EN                              ಬಿಟ್(28)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_NSECURE_INT_ACCESS                        ಬಿಟ್(27)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_NSECURE_LOCKDOWN_EN                       ಬಿಟ್(26)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_REPLACEMENT_POLICY                        ಬಿಟ್(25)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_FORCE_WR_ALLOC(X)            (((X)&0x3) * ಬಿಟ್(23)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_SHARED_ATTRIB_OVERRIDE_EN                 ಬಿಟ್(22)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_PARITY_EN                                 ಬಿಟ್(21)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_EVENT_MONITOR_BUS_EN                      ಬಿಟ್(20)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE(X)                   (((X)&0x7) * ಬಿಟ್(17) )
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_ASSOCIATIVITY                             ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_SHARED_ATTRIB_INVALIDATE_EN               ಬಿಟ್(13)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_EXCLUSIVE_CACHE_CONFIG                    ಬಿಟ್(12)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_STOREBUFDEV_ಎಲ್ಲೆ_EN                      ಬಿಟ್(11)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_ಮೇಲಿನ_PRIO_SODEV_EN                        ಬಿಟ್(10)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_ತುಂಬಿದ_ಸಾಲು_ZEROS_ENABLE                    ಬಿಟ್( 0)

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE_16K         CTRL_AUX_WAYSIZE(1)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE_32K         CTRL_AUX_WAYSIZE(2)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE_64K         CTRL_AUX_WAYSIZE(3)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE_128K        CTRL_AUX_WAYSIZE(4)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE_256K        CTRL_AUX_WAYSIZE(5)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_WAYSIZE_512K        CTRL_AUX_WAYSIZE(6)

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_ASSOCIATIVITY_8WAY   (0 * CTRL_AUX_ASSOCIATIVITY)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUX_ASSOCIATIVITY_16WAY  (1 * CTRL_AUX_ASSOCIATIVITY)

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUS_REPLPOLICY_RROBIN    (0 * CTRL_AUX_REPLACEMENT_POLICY)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_AUS_REPLPOLICY_PRAND     (1 * CTRL_AUX_REPLACEMENT_POLICY)

/* Latency */
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_RAM_LATENCY_ಗಣ(X,S)    (((X)&0x7) * ಬಿಟ್(S))
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_RAM_LATENCY_ಸಿದ್ಧತೆ(X)    CTRL_RAM_LATENCY_ಗಣ(X, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_RAM_LATENCY_READ(X)     CTRL_RAM_LATENCY_ಗಣ(X, 4)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_RAM_LATENCY_WRITE(X)    CTRL_RAM_LATENCY_ಗಣ(X, 8)

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_RAM_LATENCY(W,R,S)    ( CTRL_RAM_LATENCY_ಸಿದ್ಧತೆ(S) \
                                   | CTRL_RAM_LATENCY_READ(R)  \
                                   | CTRL_RAM_LATENCY_WRITE(W) )


/* Maintenance */
#ವ್ಯಾಖ್ಯಾನಿಸು MAINTENANCE_PENDING          ಬಿಟ್(0)

/* POWER */
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PWR_DYNAMIC_CLK_EN     ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PWR_STANDBY_ON         ಬಿಟ್(0)

/* PREFECTCH */
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_DBL_LINEFILL_EN              ಬಿಟ್(30)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_INST_PREFETCH_EN             ಬಿಟ್(29)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_ದತ್ತಾಂಶ_PREFETCH_EN             ಬಿಟ್(28)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_DBL_LINEFILL_ON_WRAP_EN      ಬಿಟ್(27)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_PREFETCH_DROP_EN             ಬಿಟ್(24)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_INCR_DBL_LINEFILL_EN         ಬಿಟ್(23)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_NOT_SAME_ID_ON_EXCL_SEQ_EN   ಬಿಟ್(21)
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL2_PFET_PREFETCH_ಸ್ಥಾನಾಂತರ(X)    ((X) * ಬಿಟ್( 0) )


ರಚನೆ l2cc_map {

    ರಚನೆ {
        uint32_t cache_id;              /* 0x000 */
        uint32_t cache_type;            /* 0x004 */
        uint32_t res[62];
    } id /* reg0 */;

    ರಚನೆ {
        uint32_t control;               /* 0x100 */
        uint32_t aux_control;           /* 0x104 */
        uint32_t ಟ್ಯಾಗ್_ram_control;       /* 0x108 */
        uint32_t ದತ್ತಾಂಶ_ram_control;      /* 0x10C */
        uint32_t res[60];
    } control /* reg1 */;

    ರಚನೆ {
        uint32_t ev_counter_ctrl;       /* 0x200 */
        uint32_t ev_counter1_cfg;       /* 0x204 */
        uint32_t ev_counter0_cfg;       /* 0x208 */
        uint32_t ev_counter1;           /* 0x20C */
        uint32_t ev_counter0;           /* 0x210 */
        uint32_t int_ಮುಖವಾಡ;              /* 0x214 */
        uint32_t int_ಮುಖವಾಡ_ಸ್ಥಿತಿ;       /* 0x218 */
        uint32_t int_raw_ಸ್ಥಿತಿ;        /* 0x21C */
        uint32_t int_clear;             /* 0x220 */
        uint32_t res[55];
    } ಅಡಚಣೆ /* reg2 */;

    ರಚನೆ {
        uint32_t res[64];
    } reg3;
    ರಚನೆ {
        uint32_t res[64];
    } reg4;
    ರಚನೆ {
        uint32_t res[64];
    } reg5;
    ರಚನೆ {
        uint32_t res[64];
    } reg6;

    ರಚನೆ {
        uint32_t res[12];
        uint32_t cache_sync;            /* 0x730 */
        uint32_t res1[15];
        uint32_t inv_pa;                /* 0x770 */
        uint32_t res2[2];
        uint32_t inv_way;               /* 0x77C */
        uint32_t res3[12];
        uint32_t clean_pa;              /* 0x7B0 */
        uint32_t res4[1];
        uint32_t clean_ಸೂಚ್ಯಂಕ;           /* 0x7B8 */
        uint32_t clean_way;             /* 0x7BC */
        uint32_t res5[12];
        uint32_t clean_inv_pa;          /* 0x7F0 */
        uint32_t res6[1];
        uint32_t clean_inv_ಸೂಚ್ಯಂಕ;       /* 0x7F8 */
        uint32_t clean_inv_way;         /* 0x7FC */
    } maintenance /* reg7 */;

    ರಚನೆ {
        uint32_t res[64];
    } reg8;

    ರಚನೆ {
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
        uint32_t ಬೀಗ_ಸಾಲು_eng;         /* 0x950 */
        uint32_t unlock_wayg;           /* 0x954 */
        uint32_t res1[42];
    } lockdown /* reg9 */;

    ರಚನೆ {
        uint32_t res[64];
    } reg10;
    ರಚನೆ {
        uint32_t res[64];
    } reg11;

    ರಚನೆ {
        uint32_t addr_filtering_ಆರಂಭ;  /* 0xC00 */
        uint32_t addr_filtering_ಕೊನೆ;    /* 0xC04 */
        uint32_t res[62];
    } filter /* reg12 */;

    ರಚನೆ {
        uint32_t res[64];
    } reg13;
    ರಚನೆ {
        uint32_t res[64];
    } reg14;

    ರಚನೆ {
        uint32_t res[16];
        uint32_t debug_ctrl;            /* 0xF40 */
        uint32_t res1[7];
        uint32_t prefetch_ctrl;         /* 0xF60 */
        uint32_t res2[7];
        uint32_t power_ctrl;            /* 0xF80 */
        uint32_t res3[31];
    } control2 /* reg15 */;
};


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ L2CC_L2C310_PPTR
#ದೋಷ L2CC_L2C310_PPTR must be defined ಗಾಗಿ ವಾಸ್ತವಿಕ ಸ್ಮರಣೆ access ಗಮ್ಯ the L2 cache controller
#ಇಲ್ಲದಿದ್ದರೆ  /* L2CC_PPTR */
ಅಸ್ಥಿರ ರಚನೆ l2cc_map *ಸ್ಥಿರಾಂಕ l2cc
    = (ಅಸ್ಥಿರ ರಚನೆ l2cc_map *)L2CC_L2C310_PPTR;
#ಆದರೆ_ಅಂತ್ಯ /* !L2CC_PPTR */


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TI_MSHIELD
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ mshield_smc(uint32_t callid, uint32_t arg1, uint32_t arg2)
{
    ನೋಂದಣಿ uint32_t _arg1 asm("r0") = arg1;
    ನೋಂದಣಿ uint32_t _arg2 asm("r1") = arg2;
    ನೋಂದಣಿ uint32_t _callid asm("r12") = callid;
    asm ಅಸ್ಥಿರ("push {r2-r12, lr}\n"
                 "dsb\n"
                 "smc #0\n"
                 "pop {r2-r12, lr}"
                 :: "r"(_callid), "r"(_arg1), "r"(_arg2));
}
#ಆದರೆ_ಅಂತ್ಯ /* TI_MSHIELD */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initL2Cache(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_DEBUG_DISABLE_L2_CACHE
    uint32_t aux;
    uint32_t ಟ್ಯಾಗ್_ram;
    uint32_t ದತ್ತಾಂಶ_ram;
    uint32_t prefetch;

    /* L2 cache must be disabled during initialisation */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TI_MSHIELD
    l2cc->control.control &= ~CTRL_CTRL_EN;
#ಆದರೆ_ಅಂತ್ಯ

    prefetch = CTRL2_PFET_INST_PREFETCH_EN | CTRL2_PFET_ದತ್ತಾಂಶ_PREFETCH_EN;
#ಆದರೆ defined(CONFIG_PLAT_IMX6)
    ಟ್ಯಾಗ್_ram  = CTRL_RAM_LATENCY(1, 2, 1);
    ದತ್ತಾಂಶ_ram = CTRL_RAM_LATENCY(1, 2, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    ಟ್ಯಾಗ್_ram  = CTRL_RAM_LATENCY(1, 1, 0);
    ದತ್ತಾಂಶ_ram = CTRL_RAM_LATENCY(1, 2, 0);
#ಆದರೆ_ಅಂತ್ಯ

    aux      = 0
               | CTRL_AUX_IPREFETCH_EN
               | CTRL_AUX_DPREFETCH_EN
               | CTRL_AUX_NSECURE_INT_ACCESS
               | CTRL_AUX_NSECURE_LOCKDOWN_EN
               | CTRL_AUX_ASSOCIATIVITY_16WAY
               | CTRL_AUS_REPLPOLICY_RROBIN;

#ಆದರೆ defined(CONFIG_PLAT_IMX6SX)
    aux |= CTRL_AUX_WAYSIZE_16K;
#ಅಥವಾಆದರೆ defined(CONFIG_PLAT_EXYNOS4) || defined(CONFIG_PLAT_IMX6) || defined(CONFIG_PLAT_ZYNQ7000) || defined(CONFIG_PLAT_ALLWINNERA20)
    aux |= CTRL_AUX_WAYSIZE_64K;
#ಅಥವಾಆದರೆ defined(OMAP4)
    aux |= CTRL_AUX_WAYSIZE_32K;
#ಇಲ್ಲದಿದ್ದರೆ /* ! (EXYNOS4 || OMAP4 || IMX6) */
#ದೋಷ Unknown platform ಗಾಗಿ L2C-310
#ಆದರೆ_ಅಂತ್ಯ /* EXYNOS4 || OMAP4 || IMX6 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 0, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_AUX, aux, 0);
    mshield_smc(MSHIELD_SMC_ROM_CTRL_LATENCY, ಟ್ಯಾಗ್_ram, ದತ್ತಾಂಶ_ram);

#ಇಲ್ಲದಿದ್ದರೆ /* !TI_MSHIELD */
    /* Direct register access */
    /* 1: Write to aux Tag RAM latentcy, Data RAM latency, prefect, power control registers  */
    l2cc->control.aux_control      = aux;
    l2cc->control.ಟ್ಯಾಗ್_ram_control  = ಟ್ಯಾಗ್_ram;
    l2cc->control.ದತ್ತಾಂಶ_ram_control = ದತ್ತಾಂಶ_ram;
    l2cc->control2.prefetch_ctrl   = prefetch;

#ಆದರೆ_ಅಂತ್ಯ /* TI_MSHIELD */

    /* 2: Invalidate by way. */
    l2cc->maintenance.inv_way = 0xffff;
    ಆಗ (l2cc->maintenance.inv_way & 0xffff);

    /* 3: write to lockdown D & I reg9 if required  */
    ಆದರೆ ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ಮುಖವಾಡ) == PL310_LOCKDOWN_BY_MASTER) {
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
    ಆದರೆ ((l2cc->id.cache_type & PL310_LOCKDOWN_BY_ಮುಖವಾಡ) == PL310_LOCKDOWN_BY_ಸಾಲು) {
        /* disable lockdown */
        l2cc->lockdown.ಬೀಗ_ಸಾಲು_eng = 0;
    }

    /* 4: write to interrupt clear register to clear any residual raw interrupts set */
    l2cc->ಅಡಚಣೆ.int_ಮುಖವಾಡ  = 0x0;
    /* 5: write to interrupt mask register if you want to enable interrupts (active high) */
    l2cc->ಅಡಚಣೆ.int_clear = ಮುಖವಾಡ(9);

    /* 6: Enable the L2 cache */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TI_MSHIELD
    /* Access secure registers through Security Middleware Call */
    mshield_smc(MSHIELD_SMC_ROM_CTRL_CTRL, 1, 0);
#ಇಲ್ಲದಿದ್ದರೆ /* !TI_MSHIELD */
    /* Direct register access */
    l2cc->control.control |= CTRL_CTRL_EN;
#ಆದರೆ_ಅಂತ್ಯ /* TI_MSHIELD */
    /* Guarantee that the setACTLR() doesn't start executing until the completion
     * of the l2cc register updates */
    dsb();

#ಆದರೆ defined(CONFIG_ARM_CORTEX_A9) && defined(CONFIG_ENABLE_A9_PREFETCHER)
    /* Set bit 1 in the ACTLR, which on the cortex-a9 is the l2 prefetch enable
     * bit. See section 4.3.10 of the Cortex-A9 Technical Reference Manual */
    ಗಣ_ACTLR(getACTLR() | ಬಿಟ್(1));
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ L2_cacheSync(ಶೂನ್ಯ)
{
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    l2cc->maintenance.cache_sync = 0;
    ಆಗ (l2cc->maintenance.cache_sync & MAINTENANCE_PENDING);
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
ಶೂನ್ಯ plat_cleanInvalidateL2Cache(ಶೂನ್ಯ)
{
    ಆದರೆ (!config_ಗಣ(CONFIG_DEBUG_DISABLE_L2_CACHE)) {
        /* Avoid normal memory writes being reordered with device memory writes */
        dmb();
        l2cc->maintenance.clean_inv_way = 0xffff;
        ಆಗ (l2cc->maintenance.clean_inv_way);
        L2_cacheSync();
    }
}

ಶೂನ್ಯ plat_cleanCache(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* Clean by way. */
    l2cc->maintenance.clean_way = 0xffff;
    ಆಗ (l2cc->maintenance.clean_way & 0xffff);
    L2_cacheSync();
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ಶೂನ್ಯ plat_cleanL2Range(paddr_t ಆರಂಭ, paddr_t ಕೊನೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ಪ್ರತಿಜ್ಞಾ(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    ಗಾಗಿ (ಆರಂಭ = L2_ಸಾಲು_ಆರಂಭ(ಆರಂಭ);
         ಆರಂಭ != L2_ಸಾಲು_ಆರಂಭ(ಕೊನೆ + L2_ಸಾಲು_ಗಾತ್ರ);
         ಆರಂಭ += L2_ಸಾಲು_ಗಾತ್ರ) {
        l2cc->maintenance.clean_pa = ಆರಂಭ;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ಶೂನ್ಯ plat_invalidateL2Range(paddr_t ಆರಂಭ, paddr_t ಕೊನೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ಪ್ರತಿಜ್ಞಾ(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    /* We assume that if this is a partial line that whoever is calling us
     * has already done the clean, so we just blindly invalidate all the lines */
    ಗಾಗಿ (ಆರಂಭ = L2_ಸಾಲು_ಆರಂಭ(ಆರಂಭ);
         ಆರಂಭ != L2_ಸಾಲು_ಆರಂಭ(ಕೊನೆ + L2_ಸಾಲು_ಗಾತ್ರ);
         ಆರಂಭ += L2_ಸಾಲು_ಗಾತ್ರ) {
        l2cc->maintenance.inv_pa = ಆರಂಭ;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

ಶೂನ್ಯ plat_cleanInvalidateL2Range(paddr_t ಆರಂಭ, paddr_t ಕೊನೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_DEBUG_DISABLE_L2_CACHE
    /* Documentation specifies this as the only possible line size */
    ಪ್ರತಿಜ್ಞಾ(((l2cc->id.cache_type >> 12) & 0x3) == 0x0);

    /* Avoid normal memory writes being reordered with device memory writes */
    dmb();
    ಗಾಗಿ (ಆರಂಭ = L2_ಸಾಲು_ಆರಂಭ(ಆರಂಭ);
         ಆರಂಭ != L2_ಸಾಲು_ಆರಂಭ(ಕೊನೆ + L2_ಸಾಲು_ಗಾತ್ರ);
         ಆರಂಭ += L2_ಸಾಲು_ಗಾತ್ರ) {
        l2cc->maintenance.clean_inv_pa = ಆರಂಭ;
        /* do not need to wait for every invalidate as 310 is atomic */
    }
    L2_cacheSync();
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_DEBUG_DISABLE_L2_CACHE */
}

