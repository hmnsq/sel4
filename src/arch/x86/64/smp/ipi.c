/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <mode/kernel/tlb.h>

#اگر_تعریف_شده ENABLE_SMP_SUPPORT

فراغ نما_handle_Remote_Call(Ipi_نما_Remote_Call_t call, کلمه_t arg0, کلمه_t arg1, کلمه_t arg2)
{
    بدل (call) {
    حالة IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (فراغ *)arg1, arg2);
        اكسر;

    حالة IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_ریشه_t *)arg0, arg1);
        اكسر;

    افتراضي:
        fail("Invalid remote call");
    }
}

#پایان_اگر /* ENABLE_SMP_SUPPORT */
