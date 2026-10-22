/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/user_access.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine/debug.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PMUSERENR_ENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNTKCTL_PL0PCTEN ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNTKCTL_PL0VCTEN ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNTKCTL_PL0VTEN  ਦੋਆਧਾਰੀ_ਅੰਕ(8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNTKCTL_PL0PTEN  ਦੋਆਧਾਰੀ_ਅੰਕ(9)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_DFR0_PMU_ਨਕਾਬ (0xful << 28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_DFR0_PMU_NONE (0xful << 28)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_PFR1_GENERIC_TIMER ਦੋਆਧਾਰੀ_ਅੰਕ(16)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_PFR0_JAZELLE_ਨਕਾਬ (0xful << 8)  /* ID_PFR0.State2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_PFR0_THUMBEE_ਨਕਾਬ (0xful << 12) /* ID_PFR0.State3 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JOSCR_CD  ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEECR_XED ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HSTR_TTEE  ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HSTR_TJDBX ਦੋਆਧਾਰੀ_ਅੰਕ(17)


ਸਥਿਰ ਖਾਲੀ disable_jazelle_user_access(ਖਾਲੀ)
{
    uint32_t v;
    ਸ਼ਬਦ_t ਅਣਵਰਤਿਆ hstr_ਬਿੱਟ = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    ਜੇ (v & ID_PFR0_JAZELLE_ਨਕਾਬ) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_ਬਿੱਟ |= HSTR_TJDBX;
    }
    ਜੇ (v & ID_PFR0_THUMBEE_ਨਕਾਬ) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_ਬਿੱਟ |= HSTR_TTEE;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    ਸੈੱਟ_HSTR(getHSTR() | hstr_ਬਿੱਟ);
#ਜੇ_ਅੰਤ
}


ਸਥਿਰ ਖਾਲੀ check_export_pmu(ਖਾਲੀ)
{
#ਜੇ defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    ਜੇ (!(readDscrCp() & DBGDSCR_SECURE_ਬਹੁਲਕ_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#ਜੇ_ਅੰਤ
}


ਸਥਿਰ ਖਾਲੀ check_export_arch_timer(ਖਾਲੀ)
{
    uint32_t v = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_PTMR_USER */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


ਖਾਲੀ armv_init_user_access(ਖਾਲੀ)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    ਜੇ ((v & ID_DFR0_PMU_ਨਕਾਬ) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    ਜੇ (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

