/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <plat/machine/hardware.h>
#समावेश <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#घोषणा EL0VCTEN BIT(1)
#घोषणा EL0PCTEN BIT(0)
#घोषणा EL0VTEN  BIT(8)
#घोषणा EL0PTEN  BIT(9)

/* bits in CNTHCTL_EL2 */
#घोषणा EL1PCEN  BIT(1)
#घोषणा EL1PCTEN BIT(0)

#घोषणा PMUSERENR_EL0_EN BIT(0)

अटल व्योम check_export_pmu(व्योम)
{
#अगर defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#पूर्ण_अगर
}

अटल व्योम check_export_arch_timer(व्योम)
{
    uint32_t val = 0;
#अगर_घोषित CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_PCNT_USER */
#अगर_घोषित CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_PTMR_USER */
#अगर_घोषित CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_VCNT_USER */
#अगर_घोषित CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#अगर_घोषित CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#पूर्ण_अगर /* CONFIG_EXPORT_PCNT_USER */
#अगर_घोषित CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#पूर्ण_अगर /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

व्योम armv_init_user_access(व्योम)
{
    check_export_pmu();
    check_export_arch_timer();
}
