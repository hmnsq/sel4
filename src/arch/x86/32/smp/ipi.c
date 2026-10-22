/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

ଶୂନ୍ୟ Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    ବାଛ (call) {
    ସ୍ଥିତି IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        ଭାଙ୍ଗ;

    ସ୍ଥିତି IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        ଭାଙ୍ଗ;

    ସ୍ଥିତି IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
