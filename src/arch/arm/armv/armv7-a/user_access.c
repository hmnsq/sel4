/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <arch/user_access.h>
#అన్తర్భావ <mode/machine/debug.h>

#నిర్వచించు PMUSERENR_ENABLE బిట్టు(0)

#నిర్వచించు CNTKCTL_PL0PCTEN బిట్టు(0)
#నిర్వచించు CNTKCTL_PL0VCTEN బిట్టు(1)
#నిర్వచించు CNTKCTL_PL0VTEN  బిట్టు(8)
#నిర్వచించు CNTKCTL_PL0PTEN  బిట్టు(9)

#నిర్వచించు ID_DFR0_PMU_ముసుగు (0xful << 28)
#నిర్వచించు ID_DFR0_PMU_NONE (0xful << 28)

#నిర్వచించు ID_PFR1_GENERIC_TIMER బిట్టు(16)

#నిర్వచించు ID_PFR0_JAZELLE_ముసుగు (0xful << 8)  /* ID_PFR0.State2 */
#నిర్వచించు ID_PFR0_THUMBEE_ముసుగు (0xful << 12) /* ID_PFR0.State3 */

#నిర్వచించు JOSCR_CD  బిట్టు(1)
#నిర్వచించు TEECR_XED బిట్టు(0)
#నిర్వచించు HSTR_TTEE  బిట్టు(16)
#నిర్వచించు HSTR_TJDBX బిట్టు(17)


స్థిర శూన్య disable_jazelle_user_access(శూన్య)
{
    uint32_t v;
    మాట_t వాడని hstr_బిట్లు = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    ఒకవేళ (v & ID_PFR0_JAZELLE_ముసుగు) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_బిట్లు |= HSTR_TJDBX;
    }
    ఒకవేళ (v & ID_PFR0_THUMBEE_ముసుగు) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_బిట్లు |= HSTR_TTEE;
    }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    సమితి_HSTR(getHSTR() | hstr_బిట్లు);
#అయితే_ముగింపు
}


స్థిర శూన్య check_export_pmu(శూన్య)
{
#ఒకవేళ defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    ఒకవేళ (!(readDscrCp() & DBGDSCR_SECURE_బహుళకం_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#అయితే_ముగింపు
}


స్థిర శూన్య check_export_arch_timer(శూన్య)
{
    uint32_t v = 0;
#నిర్వచితమైతే CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_PTMR_USER */
#నిర్వచితమైతే CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


శూన్య armv_init_user_access(శూన్య)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    ఒకవేళ ((v & ID_DFR0_PMU_ముసుగు) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    ఒకవేళ (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

