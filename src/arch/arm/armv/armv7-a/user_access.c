/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <plat/machine/hardware.h>
#समावेश <arch/user_access.h>
#समावेश <mode/machine/debug.h>

#घोषणा PMUSERENR_ENABLE BIT(0)

#घोषणा CNTKCTL_PL0PCTEN BIT(0)
#घोषणा CNTKCTL_PL0VCTEN BIT(1)
#घोषणा CNTKCTL_PL0VTEN  BIT(8)
#घोषणा CNTKCTL_PL0PTEN  BIT(9)

#घोषणा ID_DFR0_PMU_MASK (0xful << 28)
#घोषणा ID_DFR0_PMU_NONE (0xful << 28)

#घोषणा ID_PFR1_GENERIC_TIMER BIT(16)

#घोषणा ID_PFR0_JAZELLE_MASK (0xful << 8)  /* ID_PFR0.State2 */
#घोषणा ID_PFR0_THUMBEE_MASK (0xful << 12) /* ID_PFR0.State3 */

#घोषणा JOSCR_CD  BIT(1)
#घोषणा TEECR_XED BIT(0)
#घोषणा HSTR_TTEE  BIT(16)
#घोषणा HSTR_TJDBX BIT(17)


अटल व्योम disable_jazelle_user_access(व्योम)
{
    uint32_t v;
    word_t UNUSED hstr_bits = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    अगर (v & ID_PFR0_JAZELLE_MASK) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_bits |= HSTR_TJDBX;
    }
    अगर (v & ID_PFR0_THUMBEE_MASK) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_bits |= HSTR_TTEE;
    }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    setHSTR(getHSTR() | hstr_bits);
#पूर्ण_अगर
}


अटल व्योम check_export_pmu(व्योम)
{
#अगर defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    अगर (!(readDscrCp() & DBGDSCR_SECURE_MODE_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#पूर्ण_अगर
}


अटल व्योम check_export_arch_timer(व्योम)
{
    uint32_t v = 0;
#अगर_घोषित CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#पूर्ण_अगर
#अगर_घोषित CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_PTMR_USER */
#अगर_घोषित CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#पूर्ण_अगर
#अगर_घोषित CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


व्योम armv_init_user_access(व्योम)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    अगर ((v & ID_DFR0_PMU_MASK) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    अगर (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

