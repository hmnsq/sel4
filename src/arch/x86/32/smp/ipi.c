/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT

فراغ ډېرپېښ_handle_Remote_Call(Ipi_ډېرپېښ_Remote_Call_t call, کلمه_t arg0, کلمه_t arg1, کلمه_t arg2)
{
    بدل (call) {
    حالة IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        اكسر;

    حالة Ipi_Remote_Call_Invalidate_پاڼه_Structure_Cache:
        invalidate_Local_پاڼه_Structure_Cache();
        اكسر;

    حالة IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        اكسر;

    افتراضي:
        fail("Invalid remote call");
    }
}

#د_که_پای /* ENABLE_SMP_SUPPORT */
