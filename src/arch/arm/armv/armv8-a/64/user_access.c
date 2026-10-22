/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <plat/machine/hardware.h>
#ضمن <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#تعریف_کن EL0VCTEN بیت(1)
#تعریف_کن EL0PCTEN بیت(0)
#تعریف_کن EL0VTEN  بیت(8)
#تعریف_کن EL0PTEN  بیت(9)

/* bits in CNTHCTL_EL2 */
#تعریف_کن EL1PCEN  بیت(1)
#تعریف_کن EL1PCTEN بیت(0)

#تعریف_کن PMUSERENR_EL0_EN بیت(0)

ساكن فراغ check_export_pmu(فراغ)
{
#إذا defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#پایان_اگر
}

ساكن فراغ check_export_arch_timer(فراغ)
{
    uint32_t val = 0;
#اگر_تعریف_شده CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#پایان_اگر /* CONFIG_EXPORT_PCNT_USER */
#اگر_تعریف_شده CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#پایان_اگر /* CONFIG_EXPORT_PTMR_USER */
#اگر_تعریف_شده CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#پایان_اگر /* CONFIG_EXPORT_VCNT_USER */
#اگر_تعریف_شده CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#پایان_اگر /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#اگر_تعریف_شده CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#پایان_اگر /* CONFIG_EXPORT_PCNT_USER */
#اگر_تعریف_شده CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#پایان_اگر /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

فراغ armv_init_user_access(فراغ)
{
    check_export_pmu();
    check_export_arch_timer();
}
