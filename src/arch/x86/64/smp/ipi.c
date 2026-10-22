/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

فراغ Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    بدل (call) {
    حالة IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (فراغ *)arg1, arg2);
        اكسر;

    حالة IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_سيو_t *)arg0, arg1);
        اكسر;

    افتراضي:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
