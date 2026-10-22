/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/model/statedata.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_ਕੋਡ bool_t fpsimd_init(ਖਾਲੀ)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    ਵਾਪਸ true;
}
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */

BOOT_ਕੋਡ bool_t fpsimd_HWCapTest(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    ਜੇ (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & ਨਕਾਬ(4)) == ਨਕਾਬ(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & ਨਕਾਬ(4)) == ਨਕਾਬ(4)) {
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}
