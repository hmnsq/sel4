/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <arch/user_access.h>
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>

#ವ್ಯಾಖ್ಯಾನಿಸು PMUSERENR_ENABLE ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CNTKCTL_PL0PCTEN ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CNTKCTL_PL0VCTEN ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು CNTKCTL_PL0VTEN  ಬಿಟ್(8)
#ವ್ಯಾಖ್ಯಾನಿಸು CNTKCTL_PL0PTEN  ಬಿಟ್(9)

#ವ್ಯಾಖ್ಯಾನಿಸು ID_DFR0_PMU_ಮುಖವಾಡ (0xful << 28)
#ವ್ಯಾಖ್ಯಾನಿಸು ID_DFR0_PMU_NONE (0xful << 28)

#ವ್ಯಾಖ್ಯಾನಿಸು ID_PFR1_GENERIC_TIMER ಬಿಟ್(16)

#ವ್ಯಾಖ್ಯಾನಿಸು ID_PFR0_JAZELLE_ಮುಖವಾಡ (0xful << 8)  /* ID_PFR0.State2 */
#ವ್ಯಾಖ್ಯಾನಿಸು ID_PFR0_THUMBEE_ಮುಖವಾಡ (0xful << 12) /* ID_PFR0.State3 */

#ವ್ಯಾಖ್ಯಾನಿಸು JOSCR_CD  ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು TEECR_XED ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು HSTR_TTEE  ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು HSTR_TJDBX ಬಿಟ್(17)


ಸ್ಥಿರ ಶೂನ್ಯ disable_jazelle_user_access(ಶೂನ್ಯ)
{
    uint32_t v;
    ಶಬ್ದ_t ಬಳಸದ hstr_ಬಿಟ್ಟುಗಳು = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    ಆದರೆ (v & ID_PFR0_JAZELLE_ಮುಖವಾಡ) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_ಬಿಟ್ಟುಗಳು |= HSTR_TJDBX;
    }
    ಆದರೆ (v & ID_PFR0_THUMBEE_ಮುಖವಾಡ) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_ಬಿಟ್ಟುಗಳು |= HSTR_TTEE;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    ಗಣ_HSTR(getHSTR() | hstr_ಬಿಟ್ಟುಗಳು);
#ಆದರೆ_ಅಂತ್ಯ
}


ಸ್ಥಿರ ಶೂನ್ಯ check_export_pmu(ಶೂನ್ಯ)
{
#ಆದರೆ defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    ಆದರೆ (!(readDscrCp() & DBGDSCR_SECURE_ಬಹುಲಕ_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#ಆದರೆ_ಅಂತ್ಯ
}


ಸ್ಥಿರ ಶೂನ್ಯ check_export_arch_timer(ಶೂನ್ಯ)
{
    uint32_t v = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_PTMR_USER */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


ಶೂನ್ಯ armv_init_user_access(ಶೂನ್ಯ)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    ಆದರೆ ((v & ID_DFR0_PMU_ಮುಖವಾಡ) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    ಆದರೆ (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

