/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

শূন্য Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    বিকল্প (call) {
    পক্ষ IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        ভঙ্গ;

    পক্ষ IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        ভঙ্গ;

    পক্ষ IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        ভঙ্গ;

    স্বভাব:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
