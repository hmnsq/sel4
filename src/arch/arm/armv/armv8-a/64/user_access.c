/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EL0VCTEN ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EL0PCTEN ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EL0VTEN  ਦੋਆਧਾਰੀ_ਅੰਕ(8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EL0PTEN  ਦੋਆਧਾਰੀ_ਅੰਕ(9)

/* bits in CNTHCTL_EL2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EL1PCEN  ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EL1PCTEN ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PMUSERENR_EL0_EN ਦੋਆਧਾਰੀ_ਅੰਕ(0)

ਸਥਿਰ ਖਾਲੀ check_export_pmu(ਖਾਲੀ)
{
#ਜੇ defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ check_export_arch_timer(ਖਾਲੀ)
{
    uint32_t val = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_PCNT_USER */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_PTMR_USER */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_VCNT_USER */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_PCNT_USER */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#ਜੇ_ਅੰਤ /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

ਖਾਲੀ armv_init_user_access(ਖਾਲੀ)
{
    check_export_pmu();
    check_export_arch_timer();
}
