/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <mode/machine.h>
#అన్తర్భావ <arch/machine/fpu.h>
#అన్తర్భావ <mode/model/statedata.h>

#నిర్వచితమైతే CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_సంకేతం bool_t fpsimd_init(శూన్య)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    తిరిగి true;
}
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */

BOOT_సంకేతం bool_t fpsimd_HWCapTest(శూన్య)
{
    మాట_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    ఒకవేళ (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & ముసుగు(4)) == ముసుగు(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & ముసుగు(4)) == ముసుగు(4)) {
        తిరిగి false;
    }

    తిరిగి true;
}
