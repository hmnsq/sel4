/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <mode/machine.h>
#अन्तर्भाव <arch/machine/fpu.h>
#अन्तर्भाव <mode/model/statedata.h>

#ifdef CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_CODE bool_t fpsimd_init(शून्य)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    जइ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    प्रत्यागम true;
}
#endif /* CONFIG_HAVE_FPU */

BOOT_CODE bool_t fpsimd_HWCapTest(शून्य)
{
    word_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    जइ (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & MASK(4)) == MASK(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & MASK(4)) == MASK(4)) {
        प्रत्यागम false;
    }

    प्रत्यागम true;
}
