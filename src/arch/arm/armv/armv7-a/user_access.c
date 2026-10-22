/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <plat/machine/hardware.h>
#ضمن <arch/user_access.h>
#ضمن <mode/machine/debug.h>

#تعریف_کړه PMUSERENR_ENABLE بټ(0)

#تعریف_کړه CNTKCTL_PL0PCTEN بټ(0)
#تعریف_کړه CNTKCTL_PL0VCTEN بټ(1)
#تعریف_کړه CNTKCTL_PL0VTEN  بټ(8)
#تعریف_کړه CNTKCTL_PL0PTEN  بټ(9)

#تعریف_کړه ID_DFR0_PMU_ماسک (0xful << 28)
#تعریف_کړه ID_DFR0_PMU_NONE (0xful << 28)

#تعریف_کړه ID_PFR1_GENERIC_TIMER بټ(16)

#تعریف_کړه ID_PFR0_JAZELLE_ماسک (0xful << 8)  /* ID_PFR0.State2 */
#تعریف_کړه ID_PFR0_THUMBEE_ماسک (0xful << 12) /* ID_PFR0.State3 */

#تعریف_کړه JOSCR_CD  بټ(1)
#تعریف_کړه TEECR_XED بټ(0)
#تعریف_کړه HSTR_TTEE  بټ(16)
#تعریف_کړه HSTR_TJDBX بټ(17)


ساكن فراغ disable_jazelle_user_access(فراغ)
{
    uint32_t v;
    کلمه_t ناکارول_شوی hstr_بټونه = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    إذا (v & ID_PFR0_JAZELLE_ماسک) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_بټونه |= HSTR_TJDBX;
    }
    إذا (v & ID_PFR0_THUMBEE_ماسک) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_بټونه |= HSTR_TTEE;
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    سېټ_HSTR(getHSTR() | hstr_بټونه);
#د_که_پای
}


ساكن فراغ check_export_pmu(فراغ)
{
#إذا defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    إذا (!(readDscrCp() & DBGDSCR_SECURE_ډېرپېښ_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#د_که_پای
}


ساكن فراغ check_export_arch_timer(فراغ)
{
    uint32_t v = 0;
#که_تعریف_شوی CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#د_که_پای
#که_تعریف_شوی CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#د_که_پای /* CONFIG_EXPORT_PTMR_USER */
#که_تعریف_شوی CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#د_که_پای
#که_تعریف_شوی CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#د_که_پای /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


فراغ armv_init_user_access(فراغ)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    إذا ((v & ID_DFR0_PMU_ماسک) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    إذا (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

