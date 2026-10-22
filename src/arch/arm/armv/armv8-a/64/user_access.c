/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <arch/user_access.h>

/* bits in the CNTKCTL_EL1 */
#परिभाषय EL0VCTEN BIT(1)
#परिभाषय EL0PCTEN BIT(0)
#परिभाषय EL0VTEN  BIT(8)
#परिभाषय EL0PTEN  BIT(9)

/* bits in CNTHCTL_EL2 */
#परिभाषय EL1PCEN  BIT(1)
#परिभाषय EL1PCTEN BIT(0)

#परिभाषय PMUSERENR_EL0_EN BIT(0)

स्थिर शून्य check_export_pmu(शून्य)
{
#यदि defined CONFIG_EXPORT_PMU_USER || defined CONFIG_ENABLE_BENCHMARKS
    /* allow PL0 to access the PMU */
    uint32_t val = PMUSERENR_EL0_EN;
    MSR("PMUSERENR_EL0", val);
#यदि_अन्तः
}

स्थिर शून्य check_export_arch_timer(शून्य)
{
    uint32_t val = 0;
#यदि_परिभाषितम् CONFIG_EXPORT_PCNT_USER
    val |= EL0PCTEN;
#यदि_अन्तः /* CONFIG_EXPORT_PCNT_USER */
#यदि_परिभाषितम् CONFIG_EXPORT_PTMR_USER
    val |= EL0PTEN;
#यदि_अन्तः /* CONFIG_EXPORT_PTMR_USER */
#यदि_परिभाषितम् CONFIG_EXPORT_VCNT_USER
    val |= EL0VCTEN;
#यदि_अन्तः /* CONFIG_EXPORT_VCNT_USER */
#यदि_परिभाषितम् CONFIG_EXPORT_VTMR_USER
    val |= EL0VTEN;
#यदि_अन्तः /* CONFIG_EXPORT_VTMR_USER */
    MSR("CNTKCTL_EL1", val);

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    val = 0;
#यदि_परिभाषितम् CONFIG_EXPORT_PCNT_USER
    val |= EL1PCTEN;
#यदि_अन्तः /* CONFIG_EXPORT_PCNT_USER */
#यदि_परिभाषितम् CONFIG_EXPORT_PTMR_USER
    val |= EL1PCEN;
#यदि_अन्तः /* CONFIG_EXPORT_PTMR_USER */
    MSR("CNTHCTL_EL2", val);
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

शून्य armv_init_user_access(शून्य)
{
    check_export_pmu();
    check_export_arch_timer();
}
