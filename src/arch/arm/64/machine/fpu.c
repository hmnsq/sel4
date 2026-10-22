/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <mode/machine.h>
#ضمن <arch/machine/fpu.h>
#ضمن <mode/model/statedata.h>

#اگر_تعریف_شده CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_کد bool_t fpsimd_init(فراغ)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    أرجع true;
}
#پایان_اگر /* CONFIG_HAVE_FPU */

BOOT_کد bool_t fpsimd_HWCapTest(فراغ)
{
    کلمه_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    إذا (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & ماسک(4)) == ماسک(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & ماسک(4)) == ماسک(4)) {
        أرجع false;
    }

    أرجع true;
}
