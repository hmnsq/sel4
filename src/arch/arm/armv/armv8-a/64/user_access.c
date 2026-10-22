/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <plat/machine/hardware.h>
#ضمن <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#تعریف_کړه EL0VCTEN بټ(1)
#تعریف_کړه EL0PCTEN بټ(0)
#تعریف_کړه EL0VTEN  بټ(8)
#تعریف_کړه EL0PTEN  بټ(9)

/* bits in CNTHCTL_EL2 */
#تعریف_کړه EL1PCEN  بټ(1)
#تعریف_کړه EL1PCTEN بټ(0)

#تعریف_کړه PMUSERENR_EL0_EN بټ(0)

ساكن فراغ check_export_pmu(فراغ)
{
#إذا defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#د_که_پای
}

ساكن فراغ check_export_arch_timer(فراغ)
{
    uint32_t val = 0;
#که_تعریف_شوی CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#د_که_پای /* CONFIG_EXPORT_PCNT_USER */
#که_تعریف_شوی CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#د_که_پای /* CONFIG_EXPORT_PTMR_USER */
#که_تعریف_شوی CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#د_که_پای /* CONFIG_EXPORT_VCNT_USER */
#که_تعریف_شوی CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#د_که_پای /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#که_تعریف_شوی CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#د_که_پای /* CONFIG_EXPORT_PCNT_USER */
#که_تعریف_شوی CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#د_که_پای /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

فراغ armv_init_user_access(فراغ)
{
    check_export_pmu();
    check_export_arch_timer();
}
