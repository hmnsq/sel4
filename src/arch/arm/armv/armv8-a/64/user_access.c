/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#నిర్వచించు EL0VCTEN బిట్టు(1)
#నిర్వచించు EL0PCTEN బిట్టు(0)
#నిర్వచించు EL0VTEN  బిట్టు(8)
#నిర్వచించు EL0PTEN  బిట్టు(9)

/* bits in CNTHCTL_EL2 */
#నిర్వచించు EL1PCEN  బిట్టు(1)
#నిర్వచించు EL1PCTEN బిట్టు(0)

#నిర్వచించు PMUSERENR_EL0_EN బిట్టు(0)

స్థిర శూన్య check_export_pmu(శూన్య)
{
#ఒకవేళ defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#అయితే_ముగింపు
}

స్థిర శూన్య check_export_arch_timer(శూన్య)
{
    uint32_t val = 0;
#నిర్వచితమైతే CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_PCNT_USER */
#నిర్వచితమైతే CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_PTMR_USER */
#నిర్వచితమైతే CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_VCNT_USER */
#నిర్వచితమైతే CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#నిర్వచితమైతే CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_PCNT_USER */
#నిర్వచితమైతే CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#అయితే_ముగింపు /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

శూన్య armv_init_user_access(శూన్య)
{
    check_export_pmu();
    check_export_arch_timer();
}
