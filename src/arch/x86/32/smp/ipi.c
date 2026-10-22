/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>

#اگر_تعریف_شده ENABLE_SMP_SUPPORT

فراغ نما_handle_Remote_Call(Ipi_نما_Remote_Call_t call, کلمه_t arg0, کلمه_t arg1, کلمه_t arg2)
{
    بدل (call) {
    حالة IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        اكسر;

    حالة Ipi_Remote_Call_Invalidate_صفحه_Structure_Cache:
        invalidate_Local_صفحه_Structure_Cache();
        اكسر;

    حالة IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        اكسر;

    افتراضي:
        fail("Invalid remote call");
    }
}

#پایان_اگر /* ENABLE_SMP_SUPPORT */
