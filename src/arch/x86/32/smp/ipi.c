/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

تهی Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    انتخاب (call) {
    حالت IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        بشکن;

    حالت IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        بشکن;

    حالت IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        بشکن;

    پیشفرض:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
