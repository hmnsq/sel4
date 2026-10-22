/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>
#شامل <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

تهی Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    انتخاب (call) {
    حالت IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (تهی *)arg1, arg2);
        بشکن;

    حالت IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_ریشه_t *)arg0, arg1);
        بشکن;

    پیشفرض:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
