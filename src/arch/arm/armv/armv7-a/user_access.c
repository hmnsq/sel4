/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <arch/user_access.h>
#अन्तर्भाव <mode/machine/debug.h>

#परिभाषय PMUSERENR_ENABLE BIT(0)

#परिभाषय CNTKCTL_PL0PCTEN BIT(0)
#परिभाषय CNTKCTL_PL0VCTEN BIT(1)
#परिभाषय CNTKCTL_PL0VTEN  BIT(8)
#परिभाषय CNTKCTL_PL0PTEN  BIT(9)

#परिभाषय ID_DFR0_PMU_आवरणम् (0xful << 28)
#परिभाषय ID_DFR0_PMU_NONE (0xful << 28)

#परिभाषय ID_PFR1_GENERIC_TIMER BIT(16)

#परिभाषय ID_PFR0_JAZELLE_आवरणम् (0xful << 8)  /* ID_PFR0.State2 */
#परिभाषय ID_PFR0_THUMBEE_आवरणम् (0xful << 12) /* ID_PFR0.State3 */

#परिभाषय JOSCR_CD  BIT(1)
#परिभाषय TEECR_XED BIT(0)
#परिभाषय HSTR_TTEE  BIT(16)
#परिभाषय HSTR_TJDBX BIT(17)


स्थिर शून्य disable_jazelle_user_access(शून्य)
{
    uint32_t v;
    word_t अप्रयुक्तम् hstr_द्वयङ्काः = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    यदि (v & ID_PFR0_JAZELLE_आवरणम्) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_द्वयङ्काः |= HSTR_TJDBX;
    }
    यदि (v & ID_PFR0_THUMBEE_आवरणम्) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_द्वयङ्काः |= HSTR_TTEE;
    }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    समुच्चयः_HSTR(getHSTR() | hstr_द्वयङ्काः);
#यदि_अन्तः
}


स्थिर शून्य check_export_pmu(शून्य)
{
#यदि defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    यदि (!(readDscrCp() & DBGDSCR_SECURE_बहुलकम्_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#यदि_अन्तः
}


स्थिर शून्य check_export_arch_timer(शून्य)
{
    uint32_t v = 0;
#यदि_परिभाषितम् CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#यदि_अन्तः /* CONFIG_EXPORT_PTMR_USER */
#यदि_परिभाषितम् CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#यदि_अन्तः /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


शून्य armv_init_user_access(शून्य)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    यदि ((v & ID_DFR0_PMU_आवरणम्) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    यदि (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

