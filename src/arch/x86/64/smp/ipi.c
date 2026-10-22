/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <mode/smp/ipi.h>
#অন্তর্ভাব <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

শূন্য Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    নির্বাচন (call) {
    ক্ষেত্র IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (শূন্য *)arg1, arg2);
        বিরতি;

    ক্ষেত্র IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_মূল_t *)arg0, arg1);
        বিরতি;

    পূর্বনির্ধারিত:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
