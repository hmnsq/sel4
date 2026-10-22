/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <plat/machine/hardware.h>
#ضمن <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#وصف_ڪريو EL0VCTEN ٻٽو_انگ(1)
#وصف_ڪريو EL0PCTEN ٻٽو_انگ(0)
#وصف_ڪريو EL0VTEN  ٻٽو_انگ(8)
#وصف_ڪريو EL0PTEN  ٻٽو_انگ(9)

/* bits in CNTHCTL_EL2 */
#وصف_ڪريو EL1PCEN  ٻٽو_انگ(1)
#وصف_ڪريو EL1PCTEN ٻٽو_انگ(0)

#وصف_ڪريو PMUSERENR_EL0_EN ٻٽو_انگ(0)

ساكن فراغ check_export_pmu(فراغ)
{
#إذا defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#جيڪڏهن_پورو
}

ساكن فراغ check_export_arch_timer(فراغ)
{
    uint32_t val = 0;
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_PCNT_USER */
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_PTMR_USER */
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_VCNT_USER */
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_PCNT_USER */
#جيڪڏهن_وصف_ٿيل CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#جيڪڏهن_پورو /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

فراغ armv_init_user_access(فراغ)
{
    check_export_pmu();
    check_export_arch_timer();
}
