/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <mode/machine.h>
#অন্তর্ভাব <arch/machine/fpu.h>
#অন্তর্ভাব <mode/model/statedata.h>

#ifdef CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_CODE bool_t fpsimd_init(শূন্য)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    প্রত্যাগম true;
}
#endif /* CONFIG_HAVE_FPU */

BOOT_CODE bool_t fpsimd_HWCapTest(শূন্য)
{
    word_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    যদি (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & MASK(4)) == MASK(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & MASK(4)) == MASK(4)) {
        প্রত্যাগম false;
    }

    প্রত্যাগম true;
}
