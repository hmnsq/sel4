/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <plat/machine/hardware.h>
#شامل <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#تعریف_کرو EL0VCTEN ثنائی_ہندسہ(1)
#تعریف_کرو EL0PCTEN ثنائی_ہندسہ(0)
#تعریف_کرو EL0VTEN  ثنائی_ہندسہ(8)
#تعریف_کرو EL0PTEN  ثنائی_ہندسہ(9)

/* bits in CNTHCTL_EL2 */
#تعریف_کرو EL1PCEN  ثنائی_ہندسہ(1)
#تعریف_کرو EL1PCTEN ثنائی_ہندسہ(0)

#تعریف_کرو PMUSERENR_EL0_EN ثنائی_ہندسہ(0)

ساکن خالی check_export_pmu(خالی)
{
#اگر defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#اگر_ختم
}

ساکن خالی check_export_arch_timer(خالی)
{
    uint32_t val = 0;
#اگر_تعریف_شدہ CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#اگر_ختم /* CONFIG_EXPORT_PCNT_USER */
#اگر_تعریف_شدہ CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#اگر_ختم /* CONFIG_EXPORT_PTMR_USER */
#اگر_تعریف_شدہ CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#اگر_ختم /* CONFIG_EXPORT_VCNT_USER */
#اگر_تعریف_شدہ CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#اگر_ختم /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#اگر_تعریف_شدہ CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#اگر_ختم /* CONFIG_EXPORT_PCNT_USER */
#اگر_تعریف_شدہ CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#اگر_ختم /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

خالی armv_init_user_access(خالی)
{
    check_export_pmu();
    check_export_arch_timer();
}
