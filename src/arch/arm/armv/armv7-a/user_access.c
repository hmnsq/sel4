/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <plat/machine/hardware.h>
#ضمن <arch/user_access.h>
#ضمن <mode/machine/debug.h>

#وصف_ڪريو PMUSERENR_ENABLE ٻٽو_انگ(0)

#وصف_ڪريو CNTKCTL_PL0PCTEN ٻٽو_انگ(0)
#وصف_ڪريو CNTKCTL_PL0VCTEN ٻٽو_انگ(1)
#وصف_ڪريو CNTKCTL_PL0VTEN  ٻٽو_انگ(8)
#وصف_ڪريو CNTKCTL_PL0PTEN  ٻٽو_انگ(9)

#وصف_ڪريو ID_DFR0_PMU_نقاب (0xful << 28)
#وصف_ڪريو ID_DFR0_PMU_NONE (0xful << 28)

#وصف_ڪريو ID_PFR1_GENERIC_TIMER ٻٽو_انگ(16)

#وصف_ڪريو ID_PFR0_JAZELLE_نقاب (0xful << 8)  /* ID_PFR0.State2 */
#وصف_ڪريو ID_PFR0_THUMBEE_نقاب (0xful << 12) /* ID_PFR0.State3 */

#وصف_ڪريو JOSCR_CD  ٻٽو_انگ(1)
#وصف_ڪريو TEECR_XED ٻٽو_انگ(0)
#وصف_ڪريو HSTR_TTEE  ٻٽو_انگ(16)
#وصف_ڪريو HSTR_TJDBX ٻٽو_انگ(17)


ساكن فراغ disable_jazelle_user_access(فراغ)
{
    uint32_t v;
    لفظ_t اڻ_استعمال_ٿيل hstr_بٽ = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    إذا (v & ID_PFR0_JAZELLE_نقاب) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_بٽ |= HSTR_TJDBX;
    }
    إذا (v & ID_PFR0_THUMBEE_نقاب) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_بٽ |= HSTR_TTEE;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    سيٽ_HSTR(getHSTR() | hstr_بٽ);
#جيڪڏهن_پورو
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
    إذا (!(readDscrCp() & DBGDSCR_SECURE_گهڻائي_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#جيڪڏهن_پورو
}


ساكن فراغ check_export_arch_timer(فراغ)
{
    uint32_t v = 0;
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_PTMR_USER */
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


فراغ armv_init_user_access(فراغ)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    إذا ((v & ID_DFR0_PMU_نقاب) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    إذا (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

