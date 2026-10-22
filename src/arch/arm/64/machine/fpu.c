/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <mode/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <mode/model/statedata.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_ಸಂಕೇತ bool_t fpsimd_init(ಶೂನ್ಯ)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    ಹಿಂತಿರುಗಿ true;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

BOOT_ಸಂಕೇತ bool_t fpsimd_HWCapTest(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    ಆದರೆ (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & ಮುಖವಾಡ(4)) == ಮುಖವಾಡ(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & ಮುಖವಾಡ(4)) == ಮುಖವಾಡ(4)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}
