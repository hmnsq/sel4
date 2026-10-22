/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <mode/kernel/tlb.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT

فراغ گهڻائي_handle_Remote_Call(Ipi_گهڻائي_Remote_Call_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2)
{
    بدل (call) {
    حالة IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (فراغ *)arg1, arg2);
        اكسر;

    حالة IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_پاڙ_t *)arg0, arg1);
        اكسر;

    افتراضي:
        fail("Invalid remote call");
    }
}

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
