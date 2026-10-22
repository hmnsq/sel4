/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <mode/machine.h>
#समावेश <arch/machine/fpu.h>
#समावेश <mode/model/statedata.h>

#अगर_घोषित CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_CODE bool_t fpsimd_init(व्योम)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    वापस true;
}
#पूर्ण_अगर /* CONFIG_HAVE_FPU */

BOOT_CODE bool_t fpsimd_HWCapTest(व्योम)
{
    word_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    अगर (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & MASK(4)) == MASK(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & MASK(4)) == MASK(4)) {
        वापस false;
    }

    वापस true;
}
