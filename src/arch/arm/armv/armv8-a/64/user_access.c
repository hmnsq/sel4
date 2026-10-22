/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <plat/machine/hardware.h>
#ضمن <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#تعریف_کرو EL0VCTEN ثنائی_ہندسہ(1)
#تعریف_کرو EL0PCTEN ثنائی_ہندسہ(0)
#تعریف_کرو EL0VTEN  ثنائی_ہندسہ(8)
#تعریف_کرو EL0PTEN  ثنائی_ہندسہ(9)

/* bits in CNTHCTL_EL2 */
#تعریف_کرو EL1PCEN  ثنائی_ہندسہ(1)
#تعریف_کرو EL1PCTEN ثنائی_ہندسہ(0)

#تعریف_کرو PMUSERENR_EL0_EN ثنائی_ہندسہ(0)

ساكن فراغ check_export_pmu(فراغ)
{
#إذا defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#جے_انت
}

ساكن فراغ check_export_arch_timer(فراغ)
{
    uint32_t val = 0;
#جے_تعریف_شدہ CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#جے_انت /* CONFIG_EXPORT_PCNT_USER */
#جے_تعریف_شدہ CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#جے_انت /* CONFIG_EXPORT_PTMR_USER */
#جے_تعریف_شدہ CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#جے_انت /* CONFIG_EXPORT_VCNT_USER */
#جے_تعریف_شدہ CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#جے_انت /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#جے_تعریف_شدہ CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#جے_انت /* CONFIG_EXPORT_PCNT_USER */
#جے_تعریف_شدہ CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#جے_انت /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

فراغ armv_init_user_access(فراغ)
{
    check_export_pmu();
    check_export_arch_timer();
}
