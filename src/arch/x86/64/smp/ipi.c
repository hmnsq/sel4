/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>
#شامل <mode/kernel/tlb.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

خالی منوال_handle_Remote_Call(Ipi_منوال_Remote_Call_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2)
{
    انتخاب (call) {
    حالت IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (خالی *)arg1, arg2);
        توڑو;

    حالت IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_جڑ_t *)arg0, arg1);
        توڑو;

    طےشدہ:
        fail("Invalid remote call");
    }
}

#اگر_ختم /* ENABLE_SMP_SUPPORT */
