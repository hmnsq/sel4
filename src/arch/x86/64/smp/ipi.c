/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <mode/kernel/tlb.h>

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT

فراغ منوال_handle_Remote_Call(Ipi_منوال_Remote_Call_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2)
{
    بدل (call) {
    حالة IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (فراغ *)arg1, arg2);
        اكسر;

    حالة IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_جڑ_t *)arg0, arg1);
        اكسر;

    افتراضي:
        fail("Invalid remote call");
    }
}

#جے_انت /* ENABLE_SMP_SUPPORT */
