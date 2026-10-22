/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#ವ್ಯಾಖ್ಯಾನಿಸು EL0VCTEN ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು EL0PCTEN ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು EL0VTEN  ಬಿಟ್(8)
#ವ್ಯಾಖ್ಯಾನಿಸು EL0PTEN  ಬಿಟ್(9)

/* bits in CNTHCTL_EL2 */
#ವ್ಯಾಖ್ಯಾನಿಸು EL1PCEN  ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು EL1PCTEN ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು PMUSERENR_EL0_EN ಬಿಟ್(0)

ಸ್ಥಿರ ಶೂನ್ಯ check_export_pmu(ಶೂನ್ಯ)
{
#ಆದರೆ defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ check_export_arch_timer(ಶೂನ್ಯ)
{
    uint32_t val = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_PCNT_USER */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_PTMR_USER */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_VCNT_USER */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_PCNT_USER */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

ಶೂನ್ಯ armv_init_user_access(ಶೂನ್ಯ)
{
    check_export_pmu();
    check_export_arch_timer();
}
