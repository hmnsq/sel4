/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <mode/machine.h>
#அந்தர்பாவ <arch/machine/fpu.h>
#அந்தர்பாவ <mode/model/statedata.h>

#ifdef CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_CODE bool_t fpsimd_init(வெறுமை)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    திரும்பு true;
}
#endif /* CONFIG_HAVE_FPU */

BOOT_CODE bool_t fpsimd_HWCapTest(வெறுமை)
{
    word_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    எனில் (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & MASK(4)) == MASK(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & MASK(4)) == MASK(4)) {
        திரும்பு false;
    }

    திரும்பு true;
}
