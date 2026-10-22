/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <mode/smp/ipi.h>
#অন্তর্ভাৱ <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

শূন্য Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    ৱিকল্প (call) {
    পক্ষ IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (শূন্য *)arg1, arg2);
        ভঙ্গ;

    পক্ষ IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_মূল_t *)arg0, arg1);
        ভঙ্গ;

    স্ৱভাৱ:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
