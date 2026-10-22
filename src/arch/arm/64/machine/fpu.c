/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <mode/machine.h>
#شامل <arch/machine/fpu.h>
#شامل <mode/model/statedata.h>

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
/* Initialise the FP/SIMD for this machine. */
BOOT_رمز bool_t fpsimd_init(خالی)
{
    /* Set the FPU to lazy switch mode */
    disableFpu();
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        enableFpuEL01();
    }
    /* Non-HYP Kernel assumes FPU is always enabled for EL1: Make sure it is */
    isb();
    واپس true;
}
#اگر_ختم /* CONFIG_HAVE_FPU */

BOOT_رمز bool_t fpsimd_HWCapTest(خالی)
{
    لفظ_t id_aa64pfr0;

    /* Check if the hardware has FP and ASIMD support... */
    MRS("id_aa64pfr0_el1", id_aa64pfr0);
    اگر (((id_aa64pfr0 >> ID_AA64PFR0_EL1_FP) & نقاب(4)) == نقاب(4) ||
        ((id_aa64pfr0 >> ID_AA64PFR0_EL1_ASIMD) & نقاب(4)) == نقاب(4)) {
        واپس false;
    }

    واپس true;
}
