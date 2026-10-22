/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <plat/machine/hardware.h>
#شامل <arch/user_access.h>
#شامل <mode/machine/debug.h>

#تعریف_کرو PMUSERENR_ENABLE ثنائی_ہندسہ(0)

#تعریف_کرو CNTKCTL_PL0PCTEN ثنائی_ہندسہ(0)
#تعریف_کرو CNTKCTL_PL0VCTEN ثنائی_ہندسہ(1)
#تعریف_کرو CNTKCTL_PL0VTEN  ثنائی_ہندسہ(8)
#تعریف_کرو CNTKCTL_PL0PTEN  ثنائی_ہندسہ(9)

#تعریف_کرو ID_DFR0_PMU_نقاب (0xful << 28)
#تعریف_کرو ID_DFR0_PMU_NONE (0xful << 28)

#تعریف_کرو ID_PFR1_GENERIC_TIMER ثنائی_ہندسہ(16)

#تعریف_کرو ID_PFR0_JAZELLE_نقاب (0xful << 8)  /* ID_PFR0.State2 */
#تعریف_کرو ID_PFR0_THUMBEE_نقاب (0xful << 12) /* ID_PFR0.State3 */

#تعریف_کرو JOSCR_CD  ثنائی_ہندسہ(1)
#تعریف_کرو TEECR_XED ثنائی_ہندسہ(0)
#تعریف_کرو HSTR_TTEE  ثنائی_ہندسہ(16)
#تعریف_کرو HSTR_TJDBX ثنائی_ہندسہ(17)


ساکن خالی disable_jazelle_user_access(خالی)
{
    uint32_t v;
    لفظ_t غیر_مستعمل hstr_بٹ = 0;  /* Only read in CONFIG_ARM_HYPERVISOR_SUPPORT */

    MRC(ID_PFR0, v);
    اگر (v & ID_PFR0_JAZELLE_نقاب) {
        /* When JOSCR.CD == 0, JMCR is readable and writable for user space.
         * Clear, disable, and trap access. */
        MCR(JMCR, 0);
        MCR(JOSCR, JOSCR_CD);
        hstr_بٹ |= HSTR_TJDBX;
    }
    اگر (v & ID_PFR0_THUMBEE_نقاب) {
        /* Same as above, but for TEECR.XED == 0 and TEEHBR */
        MCR(TEEHBR, 0);
        MCR(TEECR, TEECR_XED);
        hstr_بٹ |= HSTR_TTEE;
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Trap access to config registers so guest cannot re-enable them. */
    سیٹ_HSTR(getHSTR() | hstr_بٹ);
#اگر_ختم
}


ساکن خالی check_export_pmu(خالی)
{
#اگر defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* Export performance counters */
    uint32_t v;
    MRC(PMUSERENR, v);
    v |= PMUSERENR_ENABLE;
    MCR(PMUSERENR, v);

    /* enable user-level pmu event counter if we're in secure mode */
    اگر (!(readDscrCp() & DBGDSCR_SECURE_منوال_DISABLED)) {
        MRC(DBGSDER, v);
        v |= DBGSDER_ENABLE_SECURE_USER_NON_INVASIVE_DEBUG;
        MCR(DBGSDER, v);
    }
#اگر_ختم
}


ساکن خالی check_export_arch_timer(خالی)
{
    uint32_t v = 0;
#اگر_تعریف_شدہ CONFIG_EXPORT_PCNT_USER
    v |= CNTKCTL_PL0PCTEN;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_EXPORT_PTMR_USER
    v |= CNTKCTL_PL0PTEN;
#اگر_ختم /* CONFIG_EXPORT_PTMR_USER */
#اگر_تعریف_شدہ CONFIG_EXPORT_VCNT_USER
    v |= CNTKCTL_PL0VCTEN;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_EXPORT_VTMR_USER
    v |= CNTKCTL_PL0VTEN;
#اگر_ختم /* CONFIG_EXPORT_VTMR_USER */
    MCR(CNTKCTL, v);
}


خالی armv_init_user_access(خالی)
{
    uint32_t v;

    /* Disable Jazelle DBX and Jazelle RCT (ThumbEE) */
    disable_jazelle_user_access();
    /* Performance Monitoring Unit */
    MRC(ID_DFR0, v);
    اگر ((v & ID_DFR0_PMU_نقاب) != ID_DFR0_PMU_NONE) {
        check_export_pmu();
    }
    /* Arch timers */
    MRC(ID_PFR1, v);
    اگر (v & ID_PFR1_GENERIC_TIMER) {
        check_export_arch_timer();
    }
}

